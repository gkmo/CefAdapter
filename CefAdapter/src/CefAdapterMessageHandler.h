#pragma once

#include <functional>

#include "include/wrapper/cef_message_router.h"

class CefAdapterMessageHandler : public CefMessageRouterBrowserSide::Handler 
{
private:	
	std::function<bool(int, long, long, std::string)> _callback;

    
public:
    CefAdapterMessageHandler();
    ~CefAdapterMessageHandler();    

	void OnSuccess(long queryId, std::string message);

	void OnFailure(long queryId, int errorCode, std::string message);

    virtual bool OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 queryId,
                         const CefString& request, bool persistent, CefRefPtr<Callback> callback) OVERRIDE;

	void SetQueryCallback(std::function<bool(int, long, long, std::string)> callback);
};