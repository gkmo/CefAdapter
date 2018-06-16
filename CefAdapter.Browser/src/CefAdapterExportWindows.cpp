
#include <iostream>

#include "include/cef_base.h"
#include "include/cef_browser_process_handler.h"
#include "include/cef_command_line.h"
#include "include/cef_render_process_handler.h"
#include "include/cef_resource_bundle_handler.h"
#include "include/cef_scheme.h"

#include "CefAdapterBrowserApplication.h"
#include "CefAdapterExport.h"
#include "CefAdapterEventHandler.h"

extern "C"
{
	EXPORT bool CreateApplication(int hInstance, const char* url, const char* subprocessPath, const char* logPath,
		BrowserCreatedCallback browserCreatedCallback, ContextCreatedCallback contextCreatedCallback, ExecuteJsFunctionCallback executeJsFunctionCallback)
	{
		CefEnableHighDPISupport();

		void* sandbox_info = NULL;

		CefMainArgs args(hInstance);
		CefSettings settings;
		
		settings.no_sandbox = true;
		
		CefString(&settings.log_file).FromASCII(logPath);		

		CefRefPtr<CefAdapterBrowserApplication> app(new CefAdapterBrowserApplication(url, browserCreatedCallback, contextCreatedCallback, executeJsFunctionCallback));

		if (subprocessPath) 
		{
			CefString(&settings.browser_subprocess_path).FromASCII(subprocessPath);
		}
		else
		{
			settings.single_process = true;

			if (CefExecuteProcess(args, app.get(), sandbox_info) >= 0)
			{
				return false;
			}
		}
					
		return CefInitialize(args, settings, app.get(), NULL);
	}

	EXPORT void RunMessageLoop()
	{
		CefRunMessageLoop();
	}

	EXPORT void Shutdown()
	{
		CefShutdown();
	}

	EXPORT bool ExecuteJavaScript(int browserId, const char * code)
	{		
		CefRefPtr<CefBrowser> browser = CefAdapterEventHandler::GetInstance()->GetBrowserById(browserId);

		if (browser) 
		{
			CefRefPtr<CefFrame> mainFrame = browser->GetMainFrame();

			mainFrame->ExecuteJavaScript(code, mainFrame->GetURL(), 0);

			return true;
		}

		return true;
	}

	EXPORT void CreateJsGlobalFunction(int browserId, const char* name, CefAdapterValueType returnType, int argumentsCount, CefAdapterValueType* arguments)
	{
		std::cout << "CreateJsGlobalFunction. Function Name = " << name << "; Arguments Count = " << argumentsCount << ";";

		for (int i = 0; i < argumentsCount; i++)
		{
			std::cout << " " << arguments[i];
		}

		std::cout << std::endl;

		CefRefPtr<CefBrowser> browser = CefAdapterEventHandler::GetInstance()->GetBrowserById(browserId);

		if (browser)
		{
			CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create("CreateJsGlobalFunction");

			CefRefPtr<CefListValue> messageArguments = message->GetArgumentList();

			messageArguments->SetString(0, name);
			messageArguments->SetInt(1, argumentsCount);

			browser->SendProcessMessage(PID_RENDERER, message);
		}
		else
		{
			std::cout << "Browser with Id = " << browserId << " was not found." << std::endl;
		}
	}

	EXPORT bool ShowDeveloperTools(int browserId)
	{
		CefRefPtr<CefBrowser> browser = CefAdapterEventHandler::GetInstance()->GetBrowserById(browserId);

		if (browser)
		{
			// Specify CEF browser settings here.
			CefBrowserSettings browserSettings;

			// Information used when creating the native window.
			CefWindowInfo windowInfo;
			windowInfo.width = 400;
			windowInfo.height = 400;

#if defined(OS_WIN)
			// On Windows we need to specify certain flags that will be passed to
			// CreateWindowEx().
			windowInfo.SetAsPopup(NULL, "CefAdapter.DeveloperTools");
#endif
			CefRefPtr<CefClient> client(CefAdapterEventHandler::GetInstance());

			CefPoint point;

			browser->GetHost()->ShowDevTools(windowInfo, client.get(), browserSettings, point);

			return true;
		}

		return true;
	}
}