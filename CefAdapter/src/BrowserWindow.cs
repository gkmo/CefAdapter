
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace CefAdapter
{
    public class BrowserWindow
    {
        private readonly int _id;
        private readonly Dictionary<string, Delegate> _functions;

        internal BrowserWindow(int id)
        {
            _id = id;
            _functions = new Dictionary<string, Delegate>();
        }

        public event EventHandler<BrowserContextCreatedEventArgs> ContextCreated;

        public void ExecuteJavaScript(string code)
        {
            NativeMethods.ExecuteJavaScript(_id, code);
        }

        public void ShowDeveloperTools()
        {
            NativeMethods.ShowDeveloperTools(_id);
        }

        public void RegisterFunctionHandler(string functionName, Delegate function)
        {
            NativeMethods.CreateJsGlobalFunction(_id, functionName);

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