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

            var initialized = _nativeInterface.CreateApplication(initialPage, 
                OnBrowserCreated, OnContextCreated, ExecuteJsFunctionCallback);

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

        CefAdapterValue ExecuteJsFunctionCallback(int browserId, string functionName, int argumentsCount, CefAdapterValue[] arguments)
        {
            Console.WriteLine(functionName);
            
            if (_browserWindows.TryGetValue(browserId, out var browserWindow))
            {
                browserWindow.ExecuteFunction(functionName, arguments);
            }

            return new CefAdapterValue()
            {
                ValueType = CefAdapterValueType.Void
            };
        }
    }
}