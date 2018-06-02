#pragma once

#include "include/cef_app.h"

typedef VOID(CALLBACK* ProgressCallback)(INT32, LPCTSTR);
typedef VOID(CALLBACK* ErrorCallback)(INT32, LPCTSTR);
typedef VOID(CALLBACK* BrowserCreatedCallback)(INT32);

class CefAdapterBrowserApplication : public CefApp, public CefBrowserProcessHandler
{
public:
	CefAdapterBrowserApplication(std::string url, BrowserCreatedCallback browserCreatedCallback);
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
	CefRefPtr<CefBrowser> _browser;

};

