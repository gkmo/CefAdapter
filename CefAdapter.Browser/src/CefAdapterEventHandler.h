#pragma once

#include "include/cef_client.h"
#include <list>
#include "CefAdapterApplication.h"

class CefAdapterEventHandler : public CefClient, public CefDisplayHandler, public CefLifeSpanHandler, public CefLoadHandler
{
public:
	CefAdapterEventHandler(BrowserCreatedCallback browserCreatedCallback);
	~CefAdapterEventHandler();

	// Provide access to the single global instance of this object.
	static CefAdapterEventHandler* GetInstance();

	// CefClient methods:
	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE 
	{
		return this;
	}

	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE 
	{
		return this;
	}

	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE 
	{ 
		return this; 
	}

	// CefDisplayHandler methods:
	virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) OVERRIDE;

	// CefLifeSpanHandler methods:
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

	// CefLoadHandler methods:
	virtual void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
		ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl) OVERRIDE;

	// Request that all existing browser windows close.
	void CloseAllBrowsers(bool forceClose);

	bool IsClosing() const { return _isClosing; }

	CefRefPtr<CefBrowser> GetBrowserById(int id);

private:
	// Platform-specific implementation.
	void PlatformTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title);

	// List of existing browser windows. Only accessed on the CEF UI thread.
	typedef std::list<CefRefPtr<CefBrowser>> BrowserList;

	BrowserList _browserList;

	bool _isClosing;

	BrowserCreatedCallback _browserCreatedCallback;

	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(CefAdapterEventHandler);
};
