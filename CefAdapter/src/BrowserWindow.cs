
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
            _nativeInterface.CreateJsGlobalFunction(_id, functionName);

            _functions[functionName] = function;
        }

        internal void OnContextCreated(int frameId)
        {
            ContextCreated?.Invoke(this, new BrowserContextCreatedEventArgs(this, frameId));
        }

        internal void ExecuteFunction(string functionName, JavaScriptValue[] arguments)
        {            
            if (!_functions.TryGetValue(functionName, out var function))
            {
                throw new Exception(string.Format($"Function '{functionName}' was not found"));
            }

            if (arguments != null && arguments.Length > 0)
            {
                if (arguments[0].ValueType == JavaScriptType.String)
                {
                    var myString = Marshal.PtrToStringAnsi(arguments[0].StringValue);

                    Console.WriteLine(myString);
                }
            }

            function.DynamicInvoke();
        }        
    }
}