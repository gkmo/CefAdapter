using System;
using System.Diagnostics;
using System.IO;
using System.Reflection;

namespace CefAdapter.Native
{
    internal class CefLinuxNativeInterface : ICefAdapterNativeInterface
    {
        public bool CreateApplication(string url, OnBrowserCreatedCallback browserCreatedCallback, 
            OnContextCreatedCallback contextCreatedCallback, ExecuteJsFunctionCallback executeJsFunctionCallback)
        {
            var hInstance = Process.GetCurrentProcess().Handle;
                        
            var rootDirectory = Path.GetDirectoryName(Assembly.GetEntryAssembly().Location);

            string subprocessPath = Path.Combine(rootDirectory, @"CefAdapter.Renderer");
            string browserLogs = Path.Combine(rootDirectory, "CefAdapter_Browser.log");
                        
            return CefLinuxNativeMethods.CreateApplication(url, subprocessPath, browserLogs, 
                browserCreatedCallback, contextCreatedCallback, executeJsFunctionCallback);
        }

        public void CreateJsGlobalFunction(int browserId, string name, CefAdapterValueType returnType, int argumentsCount, CefAdapterValueType[] argumentTypes)
        {
            CefLinuxNativeMethods.CreateJsGlobalFunction(browserId, name, returnType, argumentsCount, argumentTypes);
        }

        public bool ExecuteJavaScript(int browserId, string code)
        {
            return CefLinuxNativeMethods.ExecuteJavaScript(browserId, code);
        }

        public void RunMessageLoop()
        {
            CefLinuxNativeMethods.RunMessageLoop();
        }

        public bool ShowDeveloperTools(int browserId)
        {
            return CefLinuxNativeMethods.ShowDeveloperTools(browserId);
        }

        public void Shutdown()
        {
            CefLinuxNativeMethods.Shutdown();
        }
    }
}