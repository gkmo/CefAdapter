using System;
using System.Runtime.InteropServices;

namespace CefAdapter.Native
{
    internal delegate void InitializationProgressCallback(int percentage, string message);

    internal delegate void InitializationErrorCallback(int code, string message);

    internal delegate void OnBrowserCreatedCallback(int browserId);

    internal delegate void OnContextCreatedCallback(int browserId, int frameId);

    internal delegate JavaScriptValue ExecuteJsFunctionCallback(int browserId, string functionName, int argumentsCount, JavaScriptValue[] arguments);

    internal delegate void QuerySuccessCallback(string message);

    internal delegate void QueryFailureCallback(int errorCode, string message);

    internal delegate bool QueryCallback(int a, int b, long c, string d, QuerySuccessCallback successCallback, QueryFailureCallback failureCallback);

    internal interface ICefAdapterNativeInterface
    {
        bool CreateApplication(string url, OnBrowserCreatedCallback browserCreatedCallback, OnContextCreatedCallback contextCreatedCallback, 
            ExecuteJsFunctionCallback executeJsFunctionCallback, QueryCallback queryCallback);

        void RunMessageLoop();

        void Shutdown();
        bool ExecuteJavaScript(int browserId, string code);

        bool ShowDeveloperTools(int browserId);

        void CreateJsGlobalFunction(int browserId, string name);
    }
}