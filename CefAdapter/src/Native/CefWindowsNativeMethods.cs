using System;
using System.Runtime.InteropServices;

namespace CefAdapter.Native
{    
    internal static class CefWindowsNativeMethods
    {
        [DllImport("CefAdapter.Browser.dll")]
        public static extern bool CreateApplication(IntPtr hInstance, string url, string subprocessPath, string logPath,
            OnBrowserCreatedCallback onBrowserCreatedCallback, OnContextCreatedCallback contextCreatedCallback, ExecuteJsFunctionCallback executeJsFunctionCallback);            

        [DllImport("CefAdapter.Browser.dll")]
        public static extern void RunMessageLoop();

        [DllImport("CefAdapter.Browser.dll")]
        public static extern void Shutdown();

        [DllImport("CefAdapter.Browser.dll")]
        public static extern bool ExecuteJavaScript(int browserId, string code);

        [DllImport("CefAdapter.Browser.dll")]
        public static extern bool ShowDeveloperTools(int browserId);

        [DllImport("CefAdapter.Browser.dll")]
        public static extern void CreateJsGlobalFunction(int browserId, string name, CefAdapterValueType returnType, int argumentsCount, CefAdapterValueType[] argumentTypes);
    }
}
