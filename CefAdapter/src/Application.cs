using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using CefAdapter.Native;

namespace CefAdapter
{
    public class Application
    {
        private readonly Dictionary<int, BrowserWindow> _browserWindows = new Dictionary<int, BrowserWindow>();
        private readonly ICefAdapterNativeInterface _nativeInterface;

        public Application(string initialPage)
        {
            _nativeInterface = Native.CefNativeInterfaceFactory.GetCefNativeInterface();
            
            if (!initialPage.StartsWith("http://") && !initialPage.StartsWith("https://"))
            {
                var rootDirectory = Path.GetDirectoryName(Assembly.GetEntryAssembly().Location);
                initialPage = string.Format("file:///{0}", Path.GetFullPath(Path.Combine(rootDirectory, initialPage)));
            }            
            
            var initialized = _nativeInterface.CreateApplication(initialPage, 
                OnBrowserCreated, OnContextCreated, ExecuteJsFunctionCallback, QueryCallback);

            if (!initialized)
            {
                throw new Exception("Unable to initialize Cef application");
            }
        }

        public event EventHandler<BrowserCreatedEventAgrs> BrowserCreated;

        public BrowserWindow MainBrowserWindow { get; private set; }

        public void Run()
        {
            _nativeInterface.RunMessageLoop();

            _nativeInterface.Shutdown();
        }

        private void OnBrowserCreated(int browserId)
        {
            var browserWindow = new BrowserWindow(browserId, _nativeInterface);

            _browserWindows[browserId] = browserWindow;

            if (MainBrowserWindow == null)
            {
                MainBrowserWindow = browserWindow;
            }

            BrowserCreated?.Invoke(this, new BrowserCreatedEventAgrs(browserWindow));
        }   

        private void OnContextCreated(int browserId, int frameId)
        {
            if (_browserWindows.TryGetValue(browserId, out var browserWindow))
            {
                browserWindow.OnContextCreated(frameId);
            }
        }

        private JavaScriptValue ExecuteJsFunctionCallback(int browserId, string functionName, int argumentsCount, JavaScriptValue[] values)
        {
            Console.WriteLine(functionName);
            
            if (_browserWindows.TryGetValue(browserId, out var browserWindow))
            {
                browserWindow.ExecuteFunction(functionName, values);
            }

            return new JavaScriptValue()
            {
                ValueType = JavaScriptType.Undefined
            };
        }

        private bool QueryCallback(int browserId, int frameId, long queryId, string request, QuerySuccessCallback successCallback, QueryFailureCallback failureCallback)
        {
            
            Console.WriteLine($"QueryCallback {queryId} - {request}");
            
            if (request == "openDevTools")
            {
                if (_nativeInterface.ShowDeveloperTools(browserId))
                {
                    successCallback(queryId, "Opened developer tools");
                }
                else
                {
                     failureCallback(queryId, 1, "Failed to open developer tools");
                }

                return true;
            }            

            return false;
        }
    }
}