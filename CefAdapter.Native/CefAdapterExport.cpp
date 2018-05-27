// CefCore2.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "include/cef_base.h"
#include "include/cef_browser_process_handler.h"
#include "include/cef_command_line.h"
#include "include/cef_render_process_handler.h"
#include "include/cef_resource_bundle_handler.h"
#include "include/cef_scheme.h"

#include "CefAdapterApplication.h"
#include "CefAdapterExport.h"
#include "CefAdapterEventHandler.h"

extern "C"
{
	__declspec(dllexport) bool CreateApplication(HINSTANCE hInstance, const char* url, const char* subprocessPath, BrowserCreatedCallback browserCreatedCallback)
	{
		CefEnableHighDPISupport();

		void* sandbox_info = NULL;

		CefMainArgs args(hInstance);
		CefSettings settings;
		
		settings.no_sandbox = true;

		CefRefPtr<CefAdapterApplication> app(new CefAdapterApplication(url, browserCreatedCallback));

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

	__declspec(dllexport) void RunMessageLoop()
	{
		CefRunMessageLoop();
	}

	__declspec(dllexport) void Shutdown()
	{
		CefShutdown();
	}

	__declspec(dllexport) bool ExecuteJavaScript(int browserId, const char * code)
	{		
		CefRefPtr<CefBrowser> browser = CefAdapterEventHandler::GetInstance()->GetBrowserById(browserId);

		if (browser) 
		{
			CefRefPtr<CefFrame> mainFrame = browser->GetMainFrame();

			mainFrame->ExecuteJavaScript(code, mainFrame->GetURL(), 0);

			return TRUE;
		}

		return FALSE;
	}

	__declspec(dllexport) bool ShowDeveloperTools(int browserId)
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
			windowInfo.SetAsPopup(NULL, "CefCore");
#endif
			CefRefPtr<CefClient> client(CefAdapterEventHandler::GetInstance());

			CefPoint point;

			browser->GetHost()->ShowDevTools(windowInfo, client.get(), browserSettings, point);

			return TRUE;
		}

		return FALSE;
	}
}