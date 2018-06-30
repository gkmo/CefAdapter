#include <iostream>
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_helpers.h"

#include "CefAdapterBrowserApplication.h"
#include "CefAdapterEventHandler.h"

namespace
{
	CefAdapterBrowserApplication* g_ApplicationInstance = NULL;
}

CefAdapterBrowserApplication::CefAdapterBrowserApplication()
{	
	_eventHandler = new CefAdapterEventHandler();

	g_ApplicationInstance = this;
}

CefAdapterBrowserApplication::~CefAdapterBrowserApplication()
{
	g_ApplicationInstance = NULL;
}

CefAdapterBrowserApplication * CefAdapterBrowserApplication::GetInstance()
{
	return g_ApplicationInstance;
}

void CefAdapterBrowserApplication::OnContextInitialized()
{
	CEF_REQUIRE_UI_THREAD();

	std::cout << "CefAdapterBrowserApplication::OnContextInitialized()" << std::endl;	

	// Specify CEF browser settings here.
	CefBrowserSettings browserSettings;	

	CefRefPtr<CefCommandLine> commandLine = CefCommandLine::GetGlobalCommandLine();
    
    std::string url = commandLine->GetSwitchValue("url");
    if (url.empty())
    {
        url = "http://www.google.com";
    }

	// Information used when creating the native window.
	CefWindowInfo windowInfo;
	windowInfo.width = 800;
	windowInfo.height = 600;

#if defined(OS_WIN)
	// On Windows we need to specify certain flags that will be passed to
	// CreateWindowEx().
	windowInfo.SetAsPopup(NULL, "CefAdapter.Browser");
#endif

	// Create the first browser window.
	CefBrowserHost::CreateBrowser(windowInfo, _eventHandler, url, browserSettings, NULL);	
}

CefRefPtr<CefBrowser> CefAdapterBrowserApplication::GetMainBrowser()
{	
	return _mainBrowser;
}

CefRefPtr<CefAdapterEventHandler> CefAdapterBrowserApplication::GetEventHandler()
{
	return _eventHandler;
}

bool CefAdapterBrowserApplication::ShowDeveloperTools(int browserId)
{
	auto eventHandler = CefAdapterBrowserApplication::GetInstance()->GetEventHandler();

	CefRefPtr<CefBrowser> browser = eventHandler->GetBrowserById(browserId);

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
		CefRefPtr<CefClient> client(eventHandler);

		CefPoint point;

		browser->GetHost()->ShowDevTools(windowInfo, client.get(), browserSettings, point);

		return TRUE;
	}

	return FALSE;
}