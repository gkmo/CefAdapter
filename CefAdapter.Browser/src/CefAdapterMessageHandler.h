#pragma once

#include "include/wrapper/cef_message_router.h"
#include "CefAdapterBrowserApplication.h"

class CefAdapterMessageHandler : public CefMessageRouterBrowserSide::Handler 
{
private:
    QueryCallback _queryCallback;    
    
public:
    CefAdapterMessageHandler(QueryCallback queryCallback);
    ~CefAdapterMessageHandler();    

    virtual bool OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 query_id,
                         const CefString& request, bool persistent, CefRefPtr<Callback> callback) OVERRIDE;
};