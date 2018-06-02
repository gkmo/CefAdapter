using System;
using System.Runtime.InteropServices;

namespace CefAdapter.NetStandard
{
    public delegate void InitializationProgressCallback(int percentage, string message);

    public delegate void InitializationErrorCallback(int code, string message);

    public delegate void OnBrowserCreatedCallback(int browserId);

    public static class NativeMethods
    {
        [DllImport("CefAdapter.Browser.dll")]
        public static extern bool CreateApplication(IntPtr hInstance, string url, string subprocessPath, OnBrowserCreatedCallback onBrowserCreatedCallback);            

        [DllImport("CefAdapter.Browser.dll")]
        public static extern void RunMessageLoop();

        [DllImport("CefAdapter.Browser.dll")]
        public static extern void Shutdown();

        [DllImport("CefAdapter.Browser.dll")]
        public static extern bool ExecuteJavaScript(int browserId, string code);

        [DllImport("CefAdapter.Browser.dll")]
        public static extern bool ShowDeveloperTools(int browserId);
    }
}
