#include "stdafx.h"
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_helpers.h"

#include "CefAdapterApplication.h"
#include "CefAdapterEventHandler.h"

namespace
{
	CefAdapterApplication* g_ApplicationInstance = NULL;
}

CefAdapterApplication::CefAdapterApplication(std::string url, BrowserCreatedCallback browserCreatedCallback)
{
	_url = url;
	_browserCreatedCallback = browserCreatedCallback;	

	g_ApplicationInstance = this;
}

CefAdapterApplication::~CefAdapterApplication()
{
	g_ApplicationInstance = NULL;
}

CefAdapterApplication * CefAdapterApplication::GetInstance()
{
	return g_ApplicationInstance;
}

void CefAdapterApplication::OnContextInitialized()
{
	CEF_REQUIRE_UI_THREAD();

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

void CefAdapterApplication::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
	// Retrieve the context's window object.
	CefRefPtr<CefV8Value> object = context->GetGlobal();

	// Create a new V8 string value. See the "Basic JS Types" section below.
	CefRefPtr<CefV8Value> str = CefV8Value::CreateString("My Value!");

	// Add the string to the window object as "window.myval". See the "JS Objects" section below.
	object->SetValue("myval", str, V8_PROPERTY_ATTRIBUTE_NONE);	
}

CefRefPtr<CefBrowser> CefAdapterApplication::GetBrowser()
{	
	return _browser;
}
