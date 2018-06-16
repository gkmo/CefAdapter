using CefAdapter;
using System;

namespace CefAdapter.Samples.Simple
{
    class Program
    {
        static void Main(string[] args)
        {
            var application = new Application(@"..\..\..\presentation\index.html");

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

            e.BrowserWindow.ExecuteJavaScript("console.log('Executing JS from C#')");

            e.BrowserWindow.RegisterFunctionHandler("mycsharpfunc", new Action(CSharpFunction));

            //e.BrowserWindow.ShowDeveloperTools();

            _functionCreated = true;
        }

        private static void CSharpFunction()
        {
            Console.WriteLine("Called CSharpFunction!");
        }
    }
}
