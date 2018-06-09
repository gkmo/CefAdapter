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

	Logger* logger = new Logger("c:/temp/CefAdapter_Renderer.log");

	logger->Debug("Initialization", "Initializing...");

	CefEnableHighDPISupport();

	void* sandboxInfo = NULL;

	CefMainArgs mainArgs(hInstance);

	CefRefPtr<CefAdapterRendererApplication> rendererApplication(new CefAdapterRendererApplication(logger));

	return CefExecuteProcess(mainArgs, rendererApplication.get(), sandboxInfo);	
}
