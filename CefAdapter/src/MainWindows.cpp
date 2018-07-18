#include <windows.h>
#include <iostream>
#include <fstream>
#include <google/protobuf/stubs/common.h>

#include "include/cef_app.h"
#include "include/cef_browser_process_handler.h"
#include "include/cef_command_line.h"
#include "include/cef_render_process_handler.h"
#include "include/cef_resource_bundle_handler.h"
#include "include/cef_scheme.h"

#include "CefAdapterBrowserApplication.h"
#include "CefAdapterRendererApplication.h"
#include "Logger.h"
#include "IpcMessages.pb.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

    GOOGLE_PROTOBUF_VERIFY_VERSION;    

	CefEnableHighDPISupport();

    const char* PROCESS_TYPE_KEY = "type";

    void* sandbox_info = NULL;

    // Parse command-line arguments.
    auto commandLine = CefCommandLine::CreateCommandLine();
    commandLine->InitFromString(::GetCommandLineW());

    // Create a ClientApp of the correct type.
    CefRefPtr<CefApp> app;
    
    Logger logger;    

    if (!commandLine->HasSwitch(PROCESS_TYPE_KEY)) 
    {
        logger.Info("Initialization", "Initializing browser process...");

        app = new CefAdapterBrowserApplication(&logger);        
    } 
    else 
    {        
        app = new CefAdapterRendererApplication(&logger);

        logger.Info("Initialization", "Initializing renderer process...");
    }    

	CefMainArgs args(hInstance);

    // Execute the secondary process, if any.
    int exitCode = CefExecuteProcess(args, app, NULL);
    if (exitCode >= 0)
    {
        return exitCode;
    }

	CefSettings settings;
	settings.no_sandbox = true;
    settings.log_severity = LOGSEVERITY_INFO;
	CefString(&settings.log_file).FromASCII("CefAdapter.log");	

    logger.Info("Initialization", "CefInitialize...");

    if (!CefInitialize(args, settings, app.get(), sandbox_info))
    {        
        return 1;
    }

    logger.Info("Initialization", "CefRunMessageLoop...");

    CefRunMessageLoop();

    logger.Info("Initialization", "CefShutdown...");

    CefShutdown();
    
    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
