#pragma once

#include <list>
#include "include/cef_client.h"
#include "include/wrapper/cef_message_router.h"
#include "CefAdapterBrowserApplication.h"
#include "CefAdapterMessageHandler.h"

class CefAdapterEventHandler : public CefClient, public CefDisplayHandler, public CefLifeSpanHandler, public CefLoadHandler, public CefKeyboardHandler
{
public:
	CefAdapterEventHandler(BrowserCreatedCallback browserCreatedCallback, BrowserClosingCallback browserClosingCallback, 
		ContextCreatedCallback contextCreatedCallback, QueryCallback queryCallback);
	
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

	virtual CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() OVERRIDE
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

	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) OVERRIDE;

	virtual bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent& event, CefEventHandle os_event, bool* is_keyboard_shortcut) OVERRIDE;
private:
	// Platform-specific implementation.
	void PlatformTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title);

	// List of existing browser windows. Only accessed on the CEF UI thread.
	typedef std::list<CefRefPtr<CefBrowser>> BrowserList;

	BrowserList _browserList;

	bool _isClosing;

	BrowserCreatedCallback _browserCreatedCallback;
	BrowserClosingCallback _browserClosingCallback;
	ContextCreatedCallback _contextCreatedCallback;
	CefAdapterMessageHandler* _messageHandler;
  	CefRefPtr<CefMessageRouterBrowserSide> _messageRouter;

	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(CefAdapterEventHandler);
};

