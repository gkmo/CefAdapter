
#include <iostream>
#include "CefAdapterMessageHandler.h"

namespace 
{
    std::map<int64, CefRefPtr<CefMessageRouterBrowserSide::Callback>> _queries;
}

CefAdapterMessageHandler::CefAdapterMessageHandler()
{
	_callback = NULL;
}

CefAdapterMessageHandler::~CefAdapterMessageHandler()
{
    _callback = NULL;
}

void CefAdapterMessageHandler::SetQueryCallback(std::function<bool(int, long, long, std::string)> queryCallback)
{
	_callback = queryCallback;
}

void CefAdapterMessageHandler::OnSuccess(long queryId, std::string message)
{
    std::cout << "CefAdapterMessageHandler::OnSuccess = " << message << "; Query Id = " << queryId << std::endl;    

    auto callback = _queries[queryId];    

    callback->Success(message);

	_queries.erase(queryId);
}

void CefAdapterMessageHandler::OnFailure(long queryId, int errorCode, std::string message)
{
    std::cout << "CefAdapterMessageHandler::OnFailure = " << errorCode << " - " << message << "; Query Id = " << queryId << std::endl;    

    auto callback = _queries[queryId];    

    callback->Failure(errorCode, message);

	_queries.erase(queryId);
}

bool CefAdapterMessageHandler::OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 queryId,
                         const CefString& request, bool persistent, CefRefPtr<Callback> callback)
{
    std::cout << "CefAdapterMessageHandler::OnQuery = " << request.ToString() << "; Query Id = " << queryId << std::endl;    
    
	if (_callback == NULL)
	{
		return false;
	}

    _queries[queryId] = callback;

    if(_callback(browser->GetIdentifier(), frame->GetIdentifier(), queryId, request.ToString()))
    {
        return true;
    }

    _queries.erase(queryId);

    return false;
}