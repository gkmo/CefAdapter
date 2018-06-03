#include <windows.h>
#include "include/cef_app.h"
#include "include/cef_browser_process_handler.h"
#include "include/cef_command_line.h"
#include "include/cef_render_process_handler.h"
#include "include/cef_resource_bundle_handler.h"
#include "include/cef_scheme.h"
#include <iostream>

#include "CefAdapterRendererApplication.h"
#include "Logger.h"
#include <fstream>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  Logger* logger = new Logger("c:/temp/log.txt");
  logger->Debug("Initialization", "Initializing the renderer...");

  CefEnableHighDPISupport();

  void* sandbox_info = NULL;

  CefMainArgs main_args(hInstance);	

  // Specify CEF global settings here.
  CefSettings settings;

  // SimpleApp implements application-level callbacks for the browser process.
  // It will create the first browser instance in OnContextInitialized() after
  // CEF has initialized.
  CefRefPtr<CefAdapterRendererApplication> app(new CefAdapterRendererApplication);

  int exit_code = CefExecuteProcess(main_args, app.get(), sandbox_info);

  if (exit_code >= 0) 
  {
    return exit_code;
  }

  // Initialize CEF.
  CefInitialize(main_args, settings, app.get(), sandbox_info);

  // Run the CEF message loop. This will block until CefQuitMessageLoop() is
  // called.
  CefRunMessageLoop();

  // Shut down CEF.
  CefShutdown();

  return 0;   
}
