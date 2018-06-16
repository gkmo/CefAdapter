using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Reflection;

namespace CefAdapter
{
    public class Application
    {
        private readonly Dictionary<int, BrowserWindow> _browserWindows = new Dictionary<int, BrowserWindow>();

        public Application(string initialPage)
        {
            var hInstance = Process.GetCurrentProcess().Handle;
                        
            var rootDirectory = Path.GetDirectoryName(Assembly.GetEntryAssembly().Location);

            string subprocessPath = Path.Combine(rootDirectory, @"CefAdapter.Renderer.exe");
            string browserLogs = Path.Combine(rootDirectory, "CefAdapter_Browser.log");
            
            if (!initialPage.StartsWith("http://") && !initialPage.StartsWith("https://"))
            {
                initialPage = string.Format("file:///{0}", Path.GetFullPath(Path.Combine(rootDirectory, initialPage)));
            }            
            
            var initialized = NativeMethods.CreateApplication(hInstance, initialPage, subprocessPath, browserLogs, 
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
            NativeMethods.RunMessageLoop();

            NativeMethods.Shutdown();
        }

        private void OnBrowserCreated(int browserId)
        {
            var browserWindow = new BrowserWindow(browserId);

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

        private void ExecuteJsFunctionCallback(int browserId, string functionName, int argumentsCount, JavaScriptValue[] values)
        {
            Console.WriteLine(functionName);
            
            if (_browserWindows.TryGetValue(browserId, out var browserWindow))
            {
                browserWindow.ExecuteFunction(functionName, values);
            }

            // return new JavaScriptValue()
            // {
            //     ValueType = JavaScriptType.Void
            // };
        }
    }
}