
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using CefAdapter.Native;

namespace CefAdapter
{
    public class BrowserWindow
    {
        private readonly int _id;
        private readonly ICefAdapterNativeInterface _nativeInterface;
        private readonly Dictionary<string, Delegate> _functions;

        internal BrowserWindow(int id, ICefAdapterNativeInterface nativeInterface)
        {
            _id = id;
            _nativeInterface = nativeInterface;
            _functions = new Dictionary<string, Delegate>();
        }

        public event EventHandler<BrowserContextCreatedEventArgs> ContextCreated;

        public void ExecuteJavaScript(string code)
        {
            _nativeInterface.ExecuteJavaScript(_id, code);
        }

        public void ShowDeveloperTools()
        {
            _nativeInterface.ShowDeveloperTools(_id);
        }

        public void RegisterFunctionHandler(string functionName, Delegate function)
        {
            var parameters = function.Method.GetParameters();

            var arguments = new CefAdapterValueType[parameters.Length];

            for (int i = 0; i < parameters.Length; i++)
            {
                arguments[i] = ConvertToCefAdapterValue(parameters[i]);
            }

            _nativeInterface.CreateJsGlobalFunction(_id, functionName, 
                ConvertToCefAdapterValue(function.Method.ReturnParameter), arguments.Length, arguments);

            _functions[functionName] = function;
        }


        internal void OnContextCreated(int frameId)
        {
            ContextCreated?.Invoke(this, new BrowserContextCreatedEventArgs(this, frameId));
        }

        private static CefAdapterValueType ConvertToCefAdapterValue(System.Reflection.ParameterInfo parameter)
        {
            var cSharpType = parameter.ParameterType;

            if (cSharpType == typeof(byte))
            {
                return CefAdapterValueType.Byte;
            }
            else if (cSharpType == typeof(bool))
            {
                return CefAdapterValueType.Boolean;
            }
            else if (cSharpType == typeof(int))
            {
                return CefAdapterValueType.Int32;
            }
            else if (cSharpType == typeof(string))
            {
                return CefAdapterValueType.String;
            }
            else if (cSharpType == typeof(void))
            {
                return CefAdapterValueType.Void;
            }

            throw new Exception(string.Format($"Type '{cSharpType}' is not supported"));
        }

        internal void ExecuteFunction(string functionName, CefAdapterValue[] arguments)
        {
            if (_functions.TryGetValue(functionName, out var function))
            {
                function.DynamicInvoke();
            }
        }
    }
}