using System;
using System.Diagnostics;
using System.Threading;
using System.Threading.Tasks;
using CefAdapter.NetStandard;

namespace CefCoreNet
{
    class Program
    {
        static void Main(string[] args)
        {
            var hInstance = Process.GetCurrentProcess().Handle;
            string subprocessPath = null;//"C:\\Users\\goliveira\\Downloads\\cef-project\\cef-project\\build\\third_party\\cef\\cef_binary_3.3325.1756.g6d8faa4_windows64\\tests\\cefclient\\Debug\\cefclient.exe";

            var result = NativeMethods.CreateApplication(hInstance, "http://www.goolge.com", subprocessPath, OnBrowserCreated);

            if (result)
            {
                NativeMethods.RunMessageLoop();

                NativeMethods.Shutdown();
            }            
        }

        private static void OnBrowserCreated(int browserId)
        {
            Console.WriteLine($"Browser created with Id = {browserId}");

            if (browserId == 1)
            {
                Task.Run(() =>
                {

                    Thread.Sleep(10000);
                    NativeMethods.ExecuteJavaScript(browserId, "alert(window.myval);");

                    NativeMethods.ShowDeveloperTools(browserId);
                });
            }
        }        
    }
}
