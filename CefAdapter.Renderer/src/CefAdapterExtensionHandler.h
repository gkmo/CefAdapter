#pragma once

#include "include/cef_app.h"
#include "Logger.h"

class CefAdapterExtensionHandler : public CefV8Handler
{
public:
	CefAdapterExtensionHandler(CefRefPtr<CefBrowser> browser, Logger* logger)
	{
		_logger = logger;
		_browser = browser;
	}

	virtual bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments,
		CefRefPtr<CefV8Value>& retval, CefString& exception) OVERRIDE;	

private:
	IMPLEMENT_REFCOUNTING(CefAdapterExtensionHandler);

	Logger* _logger;
	CefRefPtr<CefBrowser> _browser;
};