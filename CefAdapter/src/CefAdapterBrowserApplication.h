#pragma once

#include <functional>
#include "include/cef_app.h"
#include "CefAdapterDefinitions.h"
#include "CefAdapterEventHandler.h"
#include "CefAdapterInterProcessCommunicator.h"
#include "Logger.h"

class CefAdapterBrowserApplication : public CefApp, public CefBrowserProcessHandler
{
public:
	CefAdapterBrowserApplication(Logger* logger);

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

private:
	IMPLEMENT_REFCOUNTING(CefAdapterBrowserApplication);

	bool ShowDeveloperTools(int browserId);
	bool ExecuteJavaScript(int browserId, std::string code);
	bool SetWindowIcon(int browserId, std::string iconPath);
	bool SetWindowTitle(int browserId, std::string title);

	CefAdapterInterProcessCommunicator* _ipc;
	CefRefPtr<CefAdapterEventHandler> _eventHandler;
	CefRefPtr<CefBrowser> _mainBrowser;
};

