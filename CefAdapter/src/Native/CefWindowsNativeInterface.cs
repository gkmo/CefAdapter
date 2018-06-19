using System.Diagnostics;
using System.IO;
using System.Reflection;
using System;

namespace CefAdapter.Native
{
    internal class CefWindowsNativeInterface : ICefAdapterNativeInterface
    {
        public bool CreateApplication(string url, OnBrowserCreatedCallback browserCreatedCallback, OnBrowserClosingCallback browserClosingCallback, 
            OnContextCreatedCallback contextCreatedCallback, JavaScriptRequestCallback queryCallback)
        {
            var hInstance = Process.GetCurrentProcess().Handle;
                        
            var rootDirectory = Path.GetDirectoryName(Assembly.GetEntryAssembly().Location);

            string subprocessPath = Path.Combine(rootDirectory, @"CefAdapter.Renderer.exe");
            string browserLogs = Path.Combine(rootDirectory, "CefAdapter_Browser.log");

            return CefWindowsNativeMethods.CreateApplication(hInstance, url, subprocessPath, browserLogs, 
                browserCreatedCallback, browserClosingCallback, contextCreatedCallback, queryCallback);
        }

        public void CreateJsGlobalFunction(int browserId, string name)
        {
            CefWindowsNativeMethods.CreateJsGlobalFunction(browserId, name);
        }

        public bool ExecuteJavaScript(int browserId, string code)
        {
            try 
            {
                return CefWindowsNativeMethods.ExecuteJavaScript(browserId, code);
            }
            catch(AccessViolationException ex)
            {
                Console.WriteLine("Error:");
                Console.WriteLine(ex.GetType().FullName);
            }

            return false;
        }

        public void RunMessageLoop()
        {
            CefWindowsNativeMethods.RunMessageLoop();
        }

        public bool ShowDeveloperTools(int browserId)
        {
            return CefWindowsNativeMethods.ShowDeveloperTools(browserId);
        }

        public void Shutdown()
        {
            CefWindowsNativeMethods.Shutdown();
        }
    }
}