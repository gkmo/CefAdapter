using System;
using System.Runtime.InteropServices;

namespace CefAdapter.Native
{
    internal delegate void InitializationProgressCallback(int percentage, string message);

    internal delegate void InitializationErrorCallback(int code, string message);

    internal delegate void OnBrowserCreatedCallback(int browserId);

    internal delegate void OnContextCreatedCallback(int browserId, int frameId);

    internal delegate CefAdapterValue ExecuteJsFunctionCallback(int browserId, string functionName, int argumentsCount, CefAdapterValue[] arguments);

    internal interface ICefAdapterNativeInterface
    {
        bool CreateApplication(string url, OnBrowserCreatedCallback browserCreatedCallback, OnContextCreatedCallback contextCreatedCallback, ExecuteJsFunctionCallback executeJsFunctionCallback);            

        void RunMessageLoop();

        void Shutdown();
        bool ExecuteJavaScript(int browserId, string code);

        bool ShowDeveloperTools(int browserId);

        void CreateJsGlobalFunction(int browserId, string name, CefAdapterValueType returnType, int argumentsCount, CefAdapterValueType[] argumentTypes);
    }
}