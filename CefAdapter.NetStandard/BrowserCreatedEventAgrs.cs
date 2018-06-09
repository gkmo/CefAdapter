using System;

namespace CefAdapter
{
    public class BrowserCreatedEventAgrs : EventArgs
    {
        public BrowserCreatedEventAgrs(BrowserWindow browserWindow)
        {
            BrowserWindow = browserWindow;
        }

        public BrowserWindow BrowserWindow { get; }
    }
}
