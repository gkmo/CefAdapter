#include "CefAdapterQueryCallback.h"

CefAdapterQueryCallback::CefAdapterQueryCallback(CefRefPtr<CefMessageRouterBrowserSide::Handler::Callback> callback)
{
    _callback = callback;
}

CefAdapterQueryCallback::~CefAdapterQueryCallback()
{
    _callback = NULL;
}

void CefAdapterQueryCallback::OnSuccess(const char* response)
{
    _callback->Success(response);
}

void CefAdapterQueryCallback::OnFailure(int errorCode, const char* message)
{
    _callback->Failure(errorCode, message);
}