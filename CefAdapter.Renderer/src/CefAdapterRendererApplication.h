#pragma once

#include "include/cef_app.h"

class CefAdapterRendererApplication : public CefApp, public CefRenderProcessHandler
{
public:
    CefAdapterRendererApplication();
	~CefAdapterRendererApplication();	

    virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE
	{
		return this;
	}

    virtual void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE;

	virtual void OnWebKitInitialized() OVERRIDE;

	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) OVERRIDE;

private:
	IMPLEMENT_REFCOUNTING(CefAdapterRendererApplication);
};