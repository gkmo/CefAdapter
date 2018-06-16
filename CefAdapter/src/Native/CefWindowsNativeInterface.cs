using System.Diagnostics;
using System.IO;
using System.Reflection;

namespace CefAdapter.Native
{
    internal class CefWindowsNativeInterface : ICefAdapterNativeInterface
    {
        public bool CreateApplication(string url, OnBrowserCreatedCallback browserCreatedCallback, OnContextCreatedCallback contextCreatedCallback, ExecuteJsFunctionCallback executeJsFunctionCallback)
        {
            var hInstance = Process.GetCurrentProcess().Handle;
                        
            var rootDirectory = Path.GetDirectoryName(Assembly.GetEntryAssembly().Location);

            string subprocessPath = Path.Combine(rootDirectory, @"CefAdapter.Renderer.exe");
            string browserLogs = Path.Combine(rootDirectory, "CefAdapter_Browser.log");

            return CefWindowsNativeMethods.CreateApplication(hInstance, url, subprocessPath, browserLogs, 
                browserCreatedCallback, contextCreatedCallback, executeJsFunctionCallback);
        }

        public void CreateJsGlobalFunction(int browserId, string name, CefAdapterValueType returnType, int argumentsCount, CefAdapterValueType[] argumentTypes)
        {
            throw new System.NotImplementedException();
        }

        public bool ExecuteJavaScript(int browserId, string code)
        {
            throw new System.NotImplementedException();
        }

        public void RunMessageLoop()
        {
            throw new System.NotImplementedException();
        }

        public bool ShowDeveloperTools(int browserId)
        {
            throw new System.NotImplementedException();
        }

        public void Shutdown()
        {
            throw new System.NotImplementedException();
        }
    }
}