#pragma once

#include "include/zmq.hpp"
#include "include/cef_app.h"

#include "CefAdapterBrowserApplication.h"

class CefAdapterInterProcessCommunicator
{
private:
	zmq::context_t* _context;
	zmq::socket_t* _replySocket;
	zmq::socket_t* _requestSocket;
	CefRefPtr<CefAdapterBrowserApplication> _application;

	void OnBrowserCreated(int browserId);
	void OnBrowserClosed(int browserId);
	void OnContextCreated(int browserId, long frameId);
	bool OnQuery(int browserId, long frameId, long, std::string query);

	void ListenRequests();
	
	std::string Invoke(std::string request);

public:
	CefAdapterInterProcessCommunicator(CefRefPtr<CefAdapterBrowserApplication> application);
	~CefAdapterInterProcessCommunicator();

	void WaitConnection();
	void FailedToInitializeApplication();
	void Shutdown();
};

