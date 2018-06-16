#include <iostream>
#include <fstream>

#include "include/cef_app.h"
#include "include/cef_browser_process_handler.h"
#include "include/cef_command_line.h"
#include "include/cef_render_process_handler.h"
#include "include/cef_resource_bundle_handler.h"
#include "include/cef_scheme.h"

#include "CefAdapterRendererApplication.h"
#include "Logger.h"


int main(int argc, char* argv[])
{
	Logger* logger = new Logger("CefAdapter_Renderer.log");

	logger->Debug("Initialization", "Initializing...");

	void* sandboxInfo = NULL;

	CefMainArgs mainArgs(argc, argv);

	CefRefPtr<CefAdapterRendererApplication> rendererApplication(new CefAdapterRendererApplication(logger));

	return CefExecuteProcess(mainArgs, rendererApplication.get(), sandboxInfo);	
}
