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
            //string subprocessPath = @"C:\Users\goliveira\Downloads\cef_binary_3.3359.1774.gd49d25f_windows64\build\tests\cefsimple\Debug\cefsimple.exe";
            string subprocessPath = @"C:\Users\goliveira\Source\Workspaces\CefDotNet\CefCoreNet\bin\Debug\netcoreapp2.1\CefAdapter.Renderer.exe";

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
                    NativeMethods.ExecuteJavaScript(browserId, "alert(test.myfunc());");

                    NativeMethods.ShowDeveloperTools(browserId);
                });
            }
        }        
    }
}
