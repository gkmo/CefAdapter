#pragma once

#include <list>
#include <vector>

#include "include/cef_client.h"
#include "include/wrapper/cef_message_router.h"

#include "CefAdapterMessageHandler.h"

class CefAdapterEventHandler : public CefClient, public CefDisplayHandler, public CefLifeSpanHandler, public CefLoadHandler, public CefKeyboardHandler
{
public:
	CefAdapterEventHandler();
	
	~CefAdapterEventHandler();

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

	virtual void OnFaviconURLChange(CefRefPtr<CefBrowser> browser, const std::vector<CefString>& icon_urls) OVERRIDE;

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

	void SetQueryCallback(std::function<bool(int, long, long, std::string)> queryCallback);

	void OnQuerySuccess(long queryId, std::string result);

	void OnQueryFailure(long queryId, int errorCode, std::string result);

	void SetBrowserCreatedCallback(std::function<void(int)> callback);

	void SetBrowserClosedCallback(std::function<void(int)> callback);

	void SetContextCreatedCallback(std::function<void(int,int)> callback);
private:
	// Platform-specific implementation.
	void PlatformTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title);

	// List of existing browser windows. Only accessed on the CEF UI thread.
	typedef std::list<CefRefPtr<CefBrowser>> BrowserList;

	BrowserList _browserList;

	bool _isClosing;

	std::function<void(int)> _browserCreatedCallback;
	std::function<void(int)> _browserClosedCallback;
	std::function<void(int,int)> _contextCreatedCallback;
	
	CefAdapterMessageHandler* _messageHandler;
  	CefRefPtr<CefMessageRouterBrowserSide> _messageRouter;

	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(CefAdapterEventHandler);
};

