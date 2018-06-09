#pragma once

#include "include/cef_app.h"
#include "include/wrapper/cef_message_router.h"
#include "Logger.h"

class CefAdapterRendererApplication : public CefApp, public CefRenderProcessHandler
{
public:
    CefAdapterRendererApplication(Logger* logger);
	~CefAdapterRendererApplication();	

    virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE
	{
		return this;
	}

    virtual void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE;

	virtual void OnWebKitInitialized() OVERRIDE;

	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId sourceProcess, CefRefPtr<CefProcessMessage> message) OVERRIDE;

private:	
	IMPLEMENT_REFCOUNTING(CefAdapterRendererApplication);

	Logger * _logger;
	CefRefPtr<CefMessageRouterRendererSide> _messageRouter;
};