using System;
using System.Collections.Generic;
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
            _nativeInterface = CefNativeInterfaceFactory.GetCefNativeInterface();
            
            if (!initialPage.StartsWith("http://") && !initialPage.StartsWith("https://"))
            {
                var rootDirectory = Path.GetDirectoryName(Assembly.GetEntryAssembly().Location);
                initialPage = string.Format("file:///{0}", Path.GetFullPath(Path.Combine(rootDirectory, initialPage)));
            }            
            
            var initialized = _nativeInterface.CreateApplication(initialPage, 
                OnBrowserCreated, OnBrowserClosing, OnContextCreated, JavaScriptRequestCallback);

            if (!initialized)
            {
                throw new Exception("Unable to initialize Cef application");
            }
        }

        public event EventHandler<BrowserWindowEventArgs> BrowserWindowCreated;        

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

            BrowserWindowCreated?.Invoke(this, new BrowserWindowEventArgs(browserWindow));
        }   

        private void OnBrowserClosing(int browserId)
        {
            if (_browserWindows.TryGetValue(browserId, out var browserWindow))
            {                
                browserWindow.OnClosing();

                if (MainBrowserWindow == browserWindow)
                {
                    MainBrowserWindow = null;
                }

                _browserWindows.Remove(browserId);
            }                    
        }   

        private void OnContextCreated(int browserId, int frameId)
        {
            if (_browserWindows.TryGetValue(browserId, out var browserWindow))
            {
                browserWindow.OnContextCreated(frameId);
            }
        }        

        private bool JavaScriptRequestCallback(int browserId, int frameId, long queryId, string request, 
            JavaScriptRequestSuccessCallback successCallback, JavaScriptRequestFailureCallback failureCallback)
        {            
            Console.WriteLine($"JavaScriptRequestCallback {queryId} - {request}");
            
            if (_browserWindows.TryGetValue(browserId, out var browserWindow))
            {
                return browserWindow.ProcessJavaScriptRequest(frameId, queryId, request, successCallback, failureCallback);
            }

            return false;
        }
    }
}