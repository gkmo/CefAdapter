using System;
using System.Diagnostics;
using System.IO;
using System.Reflection;

namespace CefAdapter.Native
{
    internal class CefLinuxNativeInterface : ICefAdapterNativeInterface
    {
        public bool CreateApplication(string url, OnBrowserCreatedCallback browserCreatedCallback, 
            OnContextCreatedCallback contextCreatedCallback, ExecuteJsFunctionCallback executeJsFunctionCallback, QueryCallback queryCallback)
        {
            var hInstance = Process.GetCurrentProcess().Handle;
                        
            var rootDirectory = Path.GetDirectoryName(Assembly.GetEntryAssembly().Location);

            string subprocessPath = Path.Combine(rootDirectory, @"CefAdapter.Renderer");
            string browserLogs = Path.Combine(rootDirectory, "CefAdapter_Browser.log");
                        
            return CefLinuxNativeMethods.CreateApplication(url, subprocessPath, browserLogs, 
                browserCreatedCallback, contextCreatedCallback, executeJsFunctionCallback, queryCallback);
        }

        public void CreateJsGlobalFunction(int browserId, string name)
        {
            CefLinuxNativeMethods.CreateJsGlobalFunction(browserId, name);
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