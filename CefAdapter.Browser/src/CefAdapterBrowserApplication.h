#pragma once

#include "include/cef_app.h"
#include "CefAdapterDefinitions.h"

class CefAdapterBrowserApplication : public CefApp, public CefBrowserProcessHandler
{
public:
	CefAdapterBrowserApplication(std::string url, BrowserCreatedCallback browserCreatedCallback, BrowserClosingCallback browserClosingCallback,
		ContextCreatedCallback contextCreatedCallback, QueryCallback queryCallback);

	~CefAdapterBrowserApplication();

	// Provide access to the single global instance of this object.
	static CefAdapterBrowserApplication* GetInstance();

	// CefApp methods:
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE 
	{
		return this;
	}

	// CefBrowserProcessHandler methods:
	virtual void OnContextInitialized() OVERRIDE;	

	CefRefPtr<CefBrowser> GetBrowser();

private:
	IMPLEMENT_REFCOUNTING(CefAdapterBrowserApplication);

	std::string _url;
	BrowserCreatedCallback _browserCreatedCallback;
	BrowserClosingCallback _browserClosingCallback;	
	ContextCreatedCallback _contextCreatedCallback;	
	QueryCallback _queryCallback;
	CefRefPtr<CefBrowser> _browser;
};

