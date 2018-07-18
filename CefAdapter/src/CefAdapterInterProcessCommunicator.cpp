#include <thread>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>

#include "CefAdapterInterProcessCommunicator.h"
#include "CefAdapterEventHandler.h"
#include "IpcMessages.pb.h"

static void Receive(zmq::socket_t* socket, CefAdapter::ToCefAdapterRequest* request)
{	
	zmq::message_t message;
	socket->recv(&message);

	request->ParseFromArray(message.data(), message.size());

	std::cout << "CefAdapterInterProcessCommunicator::Receive: " << request->DebugString() << std::endl;	
}

static void Receive(zmq::socket_t* socket, CefAdapter::CefAdapterReply* reply)
{	
	zmq::message_t message;
	socket->recv(&message);

	reply->ParseFromArray(message.data(), message.size());

	std::cout << "CefAdapterInterProcessCommunicator::Receive: " << reply->DebugString() << std::endl;	
}

static bool Send(zmq::socket_t* socket, CefAdapter::FromCefAdapterRequest* request)
{
	std::cout << "CefAdapterInterProcessCommunicator::Send: " << request->DebugString() << std::endl;
	
	zmq::message_t message(request->ByteSize());
	
	request->SerializeToArray(message.data(), request->ByteSize());
	
	return socket->send(message);
}


static bool Send(zmq::socket_t* socket, CefAdapter::CefAdapterReply* reply)
{
	std::cout << "CefAdapterInterProcessCommunicator::Send: " << reply->DebugString() << std::endl;
	
	zmq::message_t message(reply->ByteSize());
	
	reply->SerializeToArray(message.data(), reply->ByteSize());
	
	return socket->send(message);
}

CefAdapterInterProcessCommunicator::CefAdapterInterProcessCommunicator()
{	
	_context = new zmq::context_t();

	_replySocket = new zmq::socket_t(*_context, ZMQ_REP);
	_replySocket->bind("tcp://*:5560");
}

CefAdapterInterProcessCommunicator::~CefAdapterInterProcessCommunicator()
{
	_stopListening = true;

	_context->close();	

	if (_requestSocket != NULL)
	{
		delete _requestSocket;
	}	

	delete _replySocket;	
	delete _context;	
}

void CefAdapterInterProcessCommunicator::SetShowDeveloperToolsCallback(std::function<bool(int)> callback)
{
	_showDeveloperToolsCallback = callback;
}

void CefAdapterInterProcessCommunicator::SetQuerySuccessCallback(std::function<void(long,std::string)> callback)
{
	_querySuccessCallback = callback;
}

void CefAdapterInterProcessCommunicator::SetQueryFailureCallback(std::function<void(long,int,std::string)> callback)
{
	_queryFailureCallback = callback;
}

void CefAdapterInterProcessCommunicator::SetExecuteJavaScriptCallback(std::function<bool(int,std::string)> callback)
{
	_executeJavaScriptCallback = callback;
}

void CefAdapterInterProcessCommunicator::SetWindowIconCallback(std::function<bool(int, std::string)> callback)
{
	_setWindowIconCallback = callback;
}

void CefAdapterInterProcessCommunicator::SetWindowTitleCallback(std::function<bool(int, std::string)> callback)
{
	_setWindowTitleCallback = callback;
}

void CefAdapterInterProcessCommunicator::WaitConnection()
{
	CefAdapter::ToCefAdapterRequest request;
	CefAdapter::CefAdapterReply reply;

	Receive(_replySocket, &request);

	if (request.request_type() == CefAdapter::ToCefAdapterRequestType::Initialize)
	{
		_requestSocket = new zmq::socket_t(*_context, ZMQ_REQ);
		_requestSocket->connect("tcp://localhost:5561");		

		reply.set_reply_type(CefAdapter::ReplyType::Void);

		Send(_replySocket, &reply);

		std::thread listenThread(&CefAdapterInterProcessCommunicator::ListenRequests, this);
		listenThread.detach();
	}
	else 
	{
		CefAdapter::CefAdapterException exception;
		exception.set_error_code(1);
		exception.set_error_message("Invalid state.");

		reply.set_reply_type(CefAdapter::ReplyType::Exception);
		reply.set_allocated_exception(&exception);

		Send(_replySocket, &reply);
	}
}

void CefAdapterInterProcessCommunicator::FailedToInitializeApplication()
{
	Invoke(CefAdapter::FromCefAdapterRequestType::FailedToInitialize);
}

void CefAdapterInterProcessCommunicator::Shutdown()
{	
	Invoke(CefAdapter::FromCefAdapterRequestType::Shutdown);
}

void CefAdapterInterProcessCommunicator::OnBrowserCreated(int browserId)
{
	std::cout << "CefAdapterInterProcessCommunicator::OnBrowserCreated(" << browserId << ")" << std::endl;

	CefAdapter::FromCefAdapterRequest request;
	CefAdapter::OnBrowserCreatedRequest onBrowserCreatedRequest;
		
	onBrowserCreatedRequest.set_browser_id(browserId);

	request.set_request_type(CefAdapter::FromCefAdapterRequestType::OnBrowserCreated);
	request.set_allocated_on_browser_created(&onBrowserCreatedRequest);	

	Invoke(&request);
}

void CefAdapterInterProcessCommunicator::OnBrowserClosed(int browserId)
{
	std::cout << "CefAdapterInterProcessCommunicator::OnBrowserClosed(" << browserId << ")" << std::endl;

	CefAdapter::FromCefAdapterRequest request;
	CefAdapter::OnBrowserClosedRequest onBrowserClosedRequest;
		
	onBrowserClosedRequest.set_browser_id(browserId);

	request.set_request_type(CefAdapter::FromCefAdapterRequestType::OnBrowserClosed);
	request.set_allocated_on_browser_closed(&onBrowserClosedRequest);	

	Invoke(&request);
}

void CefAdapterInterProcessCommunicator::OnContextCreated(int browserId, long frameId)
{
	std::cout << "CefAdapterInterProcessCommunicator::OnContextCreated(" << browserId << ", " << frameId << ")" << std::endl;

	CefAdapter::FromCefAdapterRequest request;
	CefAdapter::OnContextCreatedRequest onContextCreatedRequest;
		
	onContextCreatedRequest.set_browser_id(browserId);
	onContextCreatedRequest.set_frame_id(frameId);

	request.set_request_type(CefAdapter::FromCefAdapterRequestType::OnContextCreated);
	request.set_allocated_on_context_created(&onContextCreatedRequest);	

	Invoke(&request);
}

bool CefAdapterInterProcessCommunicator::OnQuery(int browserId, long frameId, long queryId, std::string query)
{
	std::cout << "CefAdapterInterProcessCommunicator::OnQuery(" << browserId << ", " << frameId << ", " << queryId << ", " << query << ")" << std::endl;

	CefAdapter::FromCefAdapterRequest request;
	CefAdapter::CefAdapterReply reply;

	CefAdapter::OnQueryRequest onQueryRequest;
		
	onQueryRequest.set_browser_id(browserId);
	onQueryRequest.set_frame_id(frameId);

	request.set_request_type(CefAdapter::FromCefAdapterRequestType::OnQuery);
	request.set_allocated_on_query(&onQueryRequest);	

	Invoke(&request, &reply);

	return reply.boolean_value().value();
}

void CefAdapterInterProcessCommunicator::ListenRequests()
{
	while (!_stopListening)
	{
		try 
		{		
			CefAdapter::ToCefAdapterRequest request;
			CefAdapter::CefAdapterReply reply;

			Receive(_replySocket, &request);

			auto requestType = request.request_type();
			google::protobuf::BoolValue result;
			reply.set_reply_type(CefAdapter::ReplyType::Boolean);

			switch (requestType)
			{
				case CefAdapter::ToCefAdapterRequestType::ShowDeveloperTools:
					{
						int browserId = request.show_developer_tools().browser_id();
						result.set_value(_showDeveloperToolsCallback(browserId));						
					}
					break;
				case CefAdapter::ToCefAdapterRequestType::ExecuteJavaScript:
					{
						auto executeJavaScriptRequest = request.execute_javascript();
						int browserId = executeJavaScriptRequest.browser_id();
						std::string code = executeJavaScriptRequest.code();						
						result.set_value(_executeJavaScriptCallback(browserId, code));				
					}
					break;
				case CefAdapter::ToCefAdapterRequestType::QuerySuccess:
					{
						auto queryResult = request.query_success();
						long queryId = queryResult.query_id();
						std::string jsonObject = queryResult.json_object();
						_querySuccessCallback(queryId, jsonObject);
						result.set_value(true);
					}
					break;
				case CefAdapter::ToCefAdapterRequestType::QueryFailure:
					{
						auto queryResult = request.query_failure();
						long queryId = queryResult.query_id();
						int errorCode = queryResult.error_code();
						std::string errorMessage = queryResult.error_message();
						_queryFailureCallback(queryId, errorCode, errorMessage);
						result.set_value(true);
					}
					break;
				default:
					break;
			}					

			reply.set_allocated_boolean_value(&result);

			Send(_replySocket, &reply);
		}
		catch(const zmq::error_t& ex)
		{
			std::cout << "Exception ocurred while listening ZMQ requests: " << ex.what() << std::endl;
		}
	}
	
	_replySocket->close();
	_requestSocket->close();
}

void CefAdapterInterProcessCommunicator::Invoke(CefAdapter::FromCefAdapterRequestType requestType)
{
	CefAdapter::FromCefAdapterRequest request;
	CefAdapter::CefAdapterReply reply;

	request.set_request_type(requestType);

	Send(_requestSocket, &request);
	Receive(_requestSocket, &reply);
}

void CefAdapterInterProcessCommunicator::Invoke(CefAdapter::FromCefAdapterRequest* request)
{
	CefAdapter::CefAdapterReply ignoredReply;
	Invoke(request, &ignoredReply);
}

void CefAdapterInterProcessCommunicator::Invoke(CefAdapter::FromCefAdapterRequest* request, CefAdapter::CefAdapterReply* reply)
{
	Send(_requestSocket, request);
	Receive(_requestSocket, reply);
}