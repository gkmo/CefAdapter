#pragma once

#include <functional>
#include "include/cef_app.h"
#include "CefAdapterDefinitions.h"
#include "CefAdapterEventHandler.h"

class CefAdapterBrowserApplication : public CefApp, public CefBrowserProcessHandler
{
public:
	CefAdapterBrowserApplication();

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
	
	CefRefPtr<CefBrowser> GetMainBrowser();

	CefRefPtr<CefAdapterEventHandler> GetEventHandler();

	bool ShowDeveloperTools(int browserId);

	bool ExecuteJavaScript(int browserId, std::string code);

private:
	IMPLEMENT_REFCOUNTING(CefAdapterBrowserApplication);
	
	CefRefPtr<CefAdapterEventHandler> _eventHandler;
	CefRefPtr<CefBrowser> _mainBrowser;
};

