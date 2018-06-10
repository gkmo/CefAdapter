
#include <iostream>
#include "CefAdapterMessageHandler.h"

bool CefAdapterMessageHandler::OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 query_id,
                         const CefString& request, bool persistent, CefRefPtr<Callback> callback)
{
    std::cout << "CefAdapterMessageHandler::OnQuery = " << request.ToString() << std::endl;

    callback->Success("Epa");

    return true;
}