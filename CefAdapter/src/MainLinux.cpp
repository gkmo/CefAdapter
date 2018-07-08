
#include <iostream>
#include <fstream>

#include "include/cef_app.h"
#include "include/cef_browser_process_handler.h"
#include "include/cef_command_line.h"
#include "include/cef_render_process_handler.h"
#include "include/cef_resource_bundle_handler.h"
#include "include/cef_scheme.h"
#include "include/base/cef_logging.h"

#include "CefAdapterBrowserApplication.h"
#include "CefAdapterRendererApplication.h"
#include "CefAdapterInterProcessCommunicator.h"

#include <X11/Xlib.h>

int XErrorHandlerImpl(Display* display, XErrorEvent* event) 
{
    LOG(WARNING) << "X error received: "
                << "type " << event->type << ", "
                << "serial " << event->serial << ", "
                << "error_code " << static_cast<int>(event->error_code) << ", "
                << "request_code " << static_cast<int>(event->request_code)
                << ", "
                << "minor_code " << static_cast<int>(event->minor_code);
    return 0;
}

int XIOErrorHandlerImpl(Display* display) 
{
    return 0;
}

int main(int argc, char* argv[])
{
    const char* PROCESS_TYPE_KEY = "type";

    void* sandbox_info = NULL;

    // Parse command-line arguments.
    auto commandLine = CefCommandLine::CreateCommandLine();
    commandLine->InitFromArgv(argc, argv);

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

	CefMainArgs args(argc, argv);

    // Execute the secondary process, if any.
    int exitCode = CefExecuteProcess(args, app, NULL);
    if (exitCode >= 0)
    {
        return exitCode;
    }

    // Install xlib error handlers so that the application won't be terminated on non-fatal errors.
    XSetErrorHandler(XErrorHandlerImpl);
    XSetIOErrorHandler(XIOErrorHandlerImpl);

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
    
    return 0;
}
