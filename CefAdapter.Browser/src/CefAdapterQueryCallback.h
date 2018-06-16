
#pragma once

#include "include/wrapper/cef_message_router.h"

class CefAdapterQueryCallback
{
private:
    CefRefPtr<CefMessageRouterBrowserSide::Handler::Callback> _callback;

public:
    CefAdapterQueryCallback(CefRefPtr<CefMessageRouterBrowserSide::Handler::Callback> callback);
    ~CefAdapterQueryCallback();

    void OnSuccess(const char* response);
    void OnFailure(int errorCode, const char* message);
};
