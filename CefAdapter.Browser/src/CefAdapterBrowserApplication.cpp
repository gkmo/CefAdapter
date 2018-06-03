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

CefAdapterBrowserApplication::CefAdapterBrowserApplication(std::string url, BrowserCreatedCallback browserCreatedCallback)
{
	_url = url;
	_browserCreatedCallback = browserCreatedCallback;	

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

	// SimpleHandler implements browser-level callbacks.
	CefRefPtr<CefAdapterEventHandler> handler(new CefAdapterEventHandler(_browserCreatedCallback));

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

	// Create the first browser window.
	CefBrowserHost::CreateBrowser(windowInfo, handler, _url, browserSettings, NULL);	
}

CefRefPtr<CefBrowser> CefAdapterBrowserApplication::GetBrowser()
{	
	return _browser;
}
