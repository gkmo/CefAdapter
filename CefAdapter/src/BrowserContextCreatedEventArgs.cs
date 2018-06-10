using System;
using System.Collections.Generic;
using System.Text;

namespace CefAdapter
{
    public class BrowserContextCreatedEventArgs : EventArgs
    {
        public BrowserContextCreatedEventArgs(BrowserWindow browserWindow, int frameId)
        {
            BrowserWindow = browserWindow;
            FrameId = frameId;
        }

        public BrowserWindow BrowserWindow { get; }

        public int FrameId { get; }
    }
}
