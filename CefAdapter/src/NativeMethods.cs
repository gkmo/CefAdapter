using System;
using System.Runtime.InteropServices;

namespace CefAdapter
{
    internal delegate void InitializationProgressCallback(int percentage, string message);

    internal delegate void InitializationErrorCallback(int code, string message);

    internal delegate void OnBrowserCreatedCallback(int browserId);

    internal delegate void OnContextCreatedCallback(int browserId, int frameId);

    internal delegate CefAdapterValue ExecuteJsFunctionCallback(int browserId, string functionName, int argumentsCount, CefAdapterValue[] arguments);

    internal static class NativeMethods
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
