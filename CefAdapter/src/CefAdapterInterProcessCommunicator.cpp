#include <thread>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>

#include "CefAdapterInterProcessCommunicator.h"
#include "CefAdapterEventHandler.h"
#include "IpcMessages.pb.h"

CefAdapterInterProcessCommunicator::CefAdapterInterProcessCommunicator(Logger* logger)
{	
	_logger = logger;

	try 
	{
		_context = new zmq::context_t();

		_replySocket = new zmq::socket_t(*_context, ZMQ_REP);
		_replySocket->bind("tcp://*:5560");
	}
	catch(const zmq::error_t& ex)
	{		
		std::stringstream errorMessage;

		errorMessage << "Exception ocurred while binding ZMQ reply: " << ex.what() << std::endl;

		_logger->Error("Communication", errorMessage.str().c_str());
	}
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

void CefAdapterInterProcessCommunicator::SetShowDeveloperToolsCallback(std::function<void(int)> callback)
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
		CefAdapter::CefAdapterException* exception = new CefAdapter::CefAdapterException();
		exception->set_error_code(1);
		exception->set_error_message("Invalid state.");

		reply.set_reply_type(CefAdapter::ReplyType::Exception);
		reply.set_allocated_exception(exception);

		Send(_replySocket, &reply);

		reply.clear_exception();
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
	std::stringstream logMessage;

	logMessage << "CefAdapterInterProcessCommunicator::OnBrowserCreated(" << browserId << ")";

	_logger->Info("Communication", logMessage.str().c_str());

	CefAdapter::FromCefAdapterRequest request;
	CefAdapter::OnBrowserCreatedRequest* onBrowserCreatedRequest = new CefAdapter::OnBrowserCreatedRequest();
		
	onBrowserCreatedRequest->set_browser_id(browserId);

	request.set_request_type(CefAdapter::FromCefAdapterRequestType::OnBrowserCreated);
	request.set_allocated_on_browser_created(onBrowserCreatedRequest);	

	Invoke(&request);

	request.clear_on_browser_created();
}

void CefAdapterInterProcessCommunicator::OnBrowserClosed(int browserId)
{
	std::stringstream logMessage;

	logMessage << "CefAdapterInterProcessCommunicator::OnBrowserClosed(" << browserId << ")";

	_logger->Info("Communication", logMessage.str().c_str());

	CefAdapter::FromCefAdapterRequest request;
	CefAdapter::OnBrowserClosedRequest* onBrowserClosedRequest = new CefAdapter::OnBrowserClosedRequest();
		
	onBrowserClosedRequest->set_browser_id(browserId);

	request.set_request_type(CefAdapter::FromCefAdapterRequestType::OnBrowserClosed);
	request.set_allocated_on_browser_closed(onBrowserClosedRequest);

	Invoke(&request);

	request.clear_on_browser_closed();
}

void CefAdapterInterProcessCommunicator::OnContextCreated(int browserId, long frameId)
{
	std::stringstream logMessage;

	logMessage << "CefAdapterInterProcessCommunicator::OnContextCreated(" << browserId << ", " << frameId << ")";

	_logger->Info("Communication", logMessage.str().c_str());

	CefAdapter::FromCefAdapterRequest request;
	CefAdapter::OnContextCreatedRequest* onContextCreatedRequest = new CefAdapter::OnContextCreatedRequest();
		
	onContextCreatedRequest->set_browser_id(browserId);
	onContextCreatedRequest->set_frame_id(frameId);

	request.set_request_type(CefAdapter::FromCefAdapterRequestType::OnContextCreated);
	request.set_allocated_on_context_created(onContextCreatedRequest);	

	Invoke(&request);

	request.clear_on_context_created();
}

bool CefAdapterInterProcessCommunicator::OnQuery(int browserId, long frameId, long queryId, std::string query)
{
	std::stringstream logMessage;

	logMessage << "CefAdapterInterProcessCommunicator::OnQuery(" << browserId << ", " << frameId << ", " << queryId << ", " << query << ")";

	_logger->Info("Communication", logMessage.str().c_str());

	CefAdapter::FromCefAdapterRequest request;
	CefAdapter::CefAdapterReply reply;

	CefAdapter::OnQueryRequest* onQueryRequest = new CefAdapter::OnQueryRequest();
		
	onQueryRequest->set_browser_id(browserId);
	onQueryRequest->set_frame_id(frameId);
	onQueryRequest->set_query_id(queryId);
	onQueryRequest->set_query(query);

	request.set_request_type(CefAdapter::FromCefAdapterRequestType::OnQuery);
	request.set_allocated_on_query(onQueryRequest);

	Invoke(&request, &reply);

	request.clear_on_query();

	return true;
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

			reply.set_reply_type(CefAdapter::ReplyType::Void);

			Send(_replySocket, &reply);
		
			switch (requestType)
			{
				case CefAdapter::ToCefAdapterRequestType::ShowDeveloperTools:
					{
						int browserId = request.show_developer_tools().browser_id();
						_showDeveloperToolsCallback(browserId);
					}
					break;
				case CefAdapter::ToCefAdapterRequestType::ExecuteJavaScript:
					{
						auto executeJavaScriptRequest = request.execute_javascript();
						int browserId = executeJavaScriptRequest.browser_id();
						std::string code = executeJavaScriptRequest.code();						
						_executeJavaScriptCallback(browserId, code);
					}
					break;
				case CefAdapter::ToCefAdapterRequestType::QuerySuccess:
					{
						auto queryResult = request.query_success();
						long queryId = queryResult.query_id();
						std::string jsonObject = queryResult.json_object();
						_querySuccessCallback(queryId, jsonObject);						
					}
					break;
				case CefAdapter::ToCefAdapterRequestType::QueryFailure:
					{
						auto queryResult = request.query_failure();
						long queryId = queryResult.query_id();
						int errorCode = queryResult.error_code();
						std::string errorMessage = queryResult.error_message();
						_queryFailureCallback(queryId, errorCode, errorMessage);						
					}
					break;
				default:
					break;
			}								
		}
		catch(const zmq::error_t& ex)
		{
			std::stringstream errorMessage;

			errorMessage << "Exception ocurred while listening ZMQ requests: " << ex.what();

			_logger->Error("Communication", errorMessage.str().c_str());
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

void CefAdapterInterProcessCommunicator::Receive(zmq::socket_t* socket, CefAdapter::ToCefAdapterRequest* request)
{	
	zmq::message_t message;
	socket->recv(&message);

	request->ParseFromArray(message.data(), message.size());

	std::stringstream logMessage;

	logMessage << "Received Request: " << request->request_type();	
	
	_logger->Info("Communication", logMessage.str().c_str());	
}

void CefAdapterInterProcessCommunicator::Receive(zmq::socket_t* socket, CefAdapter::CefAdapterReply* reply)
{	
	zmq::message_t message;
	socket->recv(&message);

	reply->ParseFromArray(message.data(), message.size());

	std::stringstream logMessage;

	logMessage << "Received Reply: " << reply->reply_type();	

	_logger->Info("Communication", logMessage.str().c_str());	
}

bool CefAdapterInterProcessCommunicator::Send(zmq::socket_t* socket, CefAdapter::FromCefAdapterRequest* request)
{	
	std::stringstream logMessage;

	logMessage << "Sent Request: " << request->request_type();	

	_logger->Info("Communication", logMessage.str().c_str());

	zmq::message_t message(request->ByteSize());
	
	request->SerializeToArray(message.data(), request->ByteSize());
	
	return socket->send(message);
}

bool CefAdapterInterProcessCommunicator::Send(zmq::socket_t* socket, CefAdapter::CefAdapterReply* reply)
{	
	std::stringstream logMessage;

	logMessage << "Sent Reply: " << reply->reply_type();	

	_logger->Info("Communication", logMessage.str().c_str());

	zmq::message_t message(reply->ByteSize());
	
	reply->SerializeToArray(message.data(), reply->ByteSize());
	
	return socket->send(message);
}

