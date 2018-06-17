
#include <iostream>
#include "CefAdapterMessageHandler.h"
#include "CefAdapterQueryCallback.h"

CefAdapterMessageHandler::CefAdapterMessageHandler(QueryCallback queryCallback)
{
    _queryCallback = queryCallback;
}

CefAdapterMessageHandler::~CefAdapterMessageHandler()
{
    _queryCallback = NULL;
}

void OnSuccess(const char* message)
{

}

void OnFailure(int errorCode, const char* message)
{

}

bool CefAdapterMessageHandler::OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 query_id,
                         const CefString& request, bool persistent, CefRefPtr<Callback> callback)
{
    std::cout << "CefAdapterMessageHandler::OnQuery = " << request.ToString() << std::endl;    
    
    return _queryCallback(browser->GetIdentifier(), frame->GetIdentifier(), query_id, request.ToString().c_str(), &OnSuccess, &OnFailure);
}