using System.Runtime.InteropServices;

namespace CefAdapter.Native
{
    internal class CefNativeInterfaceFactory
    {
        private static ICefAdapterNativeInterface _instance;

        public static ICefAdapterNativeInterface GetCefNativeInterface()
        {
            if (_instance == null)
            {
                if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
                {
                    _instance = new CefLinuxNativeInterface();
                }
                else if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
                {
                    _instance = new CefWindowsNativeInterface();
                }
            }

            return _instance;
        }
    }
}