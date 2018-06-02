#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_helpers.h"

#include "CefAdapterRendererApplication.h"

namespace
{
	CefAdapterRendererApplication* g_ApplicationInstance = NULL;
}

CefAdapterRendererApplication::CefAdapterRendererApplication()
{
	g_ApplicationInstance = this;
}

CefAdapterRendererApplication::~CefAdapterRendererApplication()
{
	g_ApplicationInstance = NULL;
}

void CefAdapterRendererApplication::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
	// Retrieve the context's window object.
	CefRefPtr<CefV8Value> object = context->GetGlobal();

	// Create a new V8 string value. See the "Basic JS Types" section below.
	CefRefPtr<CefV8Value> str = CefV8Value::CreateString("My Value!");

	// Add the string to the window object as "window.myval". See the "JS Objects" section below.
	object->SetValue("myval", str, V8_PROPERTY_ATTRIBUTE_NONE);	
}