using CefAdapter;
using System;

namespace CefCoreNet
{
    class Program
    {
        static void Main(string[] args)
        {
            var application = new Application("http://www.google.com");

            application.BrowserCreated += OnBrowserCreate;

            application.Run();
        }

        private static void OnBrowserCreate(object sender, BrowserCreatedEventAgrs e)
        {
            e.BrowserWindow.ContextCreated += BrowserWindow_ContextCreated;
        }

        private static bool _functionCreated = false;

        private static void BrowserWindow_ContextCreated(object sender, BrowserContextCreatedEventArgs e)
        {
            if (_functionCreated)
            {
                return;
            }

            //e.BrowserWindow.ExecuteJavaScript("alert('Executing JS from C#')");

            e.BrowserWindow.RegisterFunctionHandler("mycsharpfunc", new Action(CSharpFunction));

            _functionCreated = true;
        }

        private static void CSharpFunction()
        {
            Console.WriteLine("Called CSharpFunction!");
        }
    }
}
