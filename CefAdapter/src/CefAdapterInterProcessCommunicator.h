#pragma once

#include <thread>

#include "include/cef_app.h"
#include "zmq.hpp"
#include "IpcMessages.pb.h"
#include "Logger.h"

class CefAdapterInterProcessCommunicator
{
public:
	CefAdapterInterProcessCommunicator(Logger* logger);
	~CefAdapterInterProcessCommunicator();

	void OnBrowserCreated(int browserId);
	void OnBrowserClosed(int browserId);
	void OnContextCreated(int browserId, long frameId);
	bool OnQuery(int browserId, long frameId, long, std::string query);

	void SetShowDeveloperToolsCallback(std::function<void(int)> callback);
	void SetQuerySuccessCallback(std::function<void(long,std::string)> callback);
	void SetQueryFailureCallback(std::function<void(long,int,std::string)> callback);
	void SetExecuteJavaScriptCallback(std::function<bool(int,std::string)> callback);
	void SetWindowIconCallback(std::function<bool(int,std::string)> callback);
	void SetWindowTitleCallback(std::function<bool(int,std::string)> callback);

	void WaitConnection();
	void FailedToInitializeApplication();
	void Shutdown();

private:
	Logger* _logger;
	bool _stopListening = false;	
	zmq::context_t* _context;
	zmq::socket_t* _replySocket;
	zmq::socket_t* _requestSocket;	
	std::function<void(int)> _showDeveloperToolsCallback;
	std::function<void(long,std::string)> _querySuccessCallback;
	std::function<void(long,int,std::string)> _queryFailureCallback;
	std::function<bool(int,std::string)> _executeJavaScriptCallback;
	std::function<bool(int,std::string)> _setWindowIconCallback;
	std::function<bool(int,std::string)> _setWindowTitleCallback;
	
	void ListenRequests();
	
	void Invoke(CefAdapter::FromCefAdapterRequestType requestType);
	void Invoke(CefAdapter::FromCefAdapterRequest* request);
	void Invoke(CefAdapter::FromCefAdapterRequest* request, CefAdapter::CefAdapterReply* reply);

	void Receive(zmq::socket_t* socket, CefAdapter::ToCefAdapterRequest* request);
	void Receive(zmq::socket_t* socket, CefAdapter::CefAdapterReply* reply);
	bool Send(zmq::socket_t* socket, CefAdapter::FromCefAdapterRequest* request);
	bool Send(zmq::socket_t* socket, CefAdapter::CefAdapterReply* reply);
};
