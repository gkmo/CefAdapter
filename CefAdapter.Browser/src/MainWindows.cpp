#include <windows.h>
#include <iostream>
#include <fstream>

#include "include/cef_app.h"
#include "include/cef_browser_process_handler.h"
#include "include/cef_command_line.h"
#include "include/cef_render_process_handler.h"
#include "include/cef_resource_bundle_handler.h"
#include "include/cef_scheme.h"

#include "CefAdapterBrowserApplication.h"
#include "CefAdapterInterProcessCommunicator.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	CefEnableHighDPISupport();

    void* sandbox_info = NULL;

    CefRefPtr<CefAdapterBrowserApplication> app(new CefAdapterBrowserApplication());

    auto ipc = new CefAdapterInterProcessCommunicator(app.get());
    
    ipc->WaitConnection();

	CefMainArgs args(hInstance);

	CefSettings settings;
	settings.no_sandbox = true;
	CefString(&settings.log_file).FromASCII("CefAdapter.Browser.log");
	CefString(&settings.browser_subprocess_path).FromASCII("CefAdapter.Renderer.exe");

    if (!CefInitialize(args, settings, app.get(), sandbox_info))
    {
        ipc->FailedToInitializeApplication();
        return 1;
    }

    CefRunMessageLoop();

    ipc->Shutdown();

    delete ipc;

    CefShutdown();    
    
    return 0;
}
