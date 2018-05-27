#pragma once

#include "include/cef_app.h"

typedef VOID(CALLBACK* ProgressCallback)(INT32, LPCTSTR);
typedef VOID(CALLBACK* ErrorCallback)(INT32, LPCTSTR);
typedef VOID(CALLBACK* BrowserCreatedCallback)(INT32);

class CefAdapterApplication : public CefApp, public CefBrowserProcessHandler, public CefRenderProcessHandler
{
public:
	CefAdapterApplication(std::string url, BrowserCreatedCallback browserCreatedCallback);
	~CefAdapterApplication();

	// Provide access to the single global instance of this object.
	static CefAdapterApplication* GetInstance();

	// CefApp methods:
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE 
	{
		return this;
	}

	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE
	{
		return this;
	}

	// CefBrowserProcessHandler methods:
	virtual void OnContextInitialized() OVERRIDE;

	virtual void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE;

	CefRefPtr<CefBrowser> GetBrowser();

private:
	IMPLEMENT_REFCOUNTING(CefAdapterApplication);

	std::string _url;
	BrowserCreatedCallback _browserCreatedCallback;	
	CefRefPtr<CefBrowser> _browser;

};

