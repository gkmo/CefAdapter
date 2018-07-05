#include <thread>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>

#include "CefAdapterInterProcessCommunicator.h"
#include "CefAdapterEventHandler.h"

template<typename Out>
void SplitString(const std::string &s, char delim, Out result)
{
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		*(result++) = item;
	}
}

std::vector<std::string> SplitString(const std::string &s, char delim)
{
	std::vector<std::string> elems;
	SplitString(s, delim, std::back_inserter(elems));
	return elems;
}

static std::string ReceiveMessage(zmq::socket_t* socket)
{
	zmq::message_t message;
	socket->recv(&message);
	auto result = std::string(static_cast<char*>(message.data()), message.size());

	std::cout << "CefAdapterInterProcessCommunicator::Receive: " << result << std::endl;

	return result;
}

static bool SendMessage(zmq::socket_t* socket, const std::string & string)
{
	std::cout << "CefAdapterInterProcessCommunicator::Send: " << string << std::endl;

	zmq::message_t message(string.size());
	memcpy(message.data(), string.data(), string.size());
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
	_replySocket->unbind("tcp://*:5560");
	_replySocket->close();

	if (_requestSocket != NULL)
	{
		_requestSocket->disconnect("tcp://*:5561");
		_requestSocket->close();
		delete _requestSocket;
	}

	_context->close();

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

void CefAdapterInterProcessCommunicator::WaitConnection()
{
	std::string message = ReceiveMessage(_replySocket);

	if (message.compare("CONNECT|") == 0)
	{
		_requestSocket = new zmq::socket_t(*_context, ZMQ_REQ);
		_requestSocket->connect("tcp://localhost:5561");

		SendMessage(_replySocket, "CONNECT|SUCCESS");

		std::thread listenThread(&CefAdapterInterProcessCommunicator::ListenRequests, this);

		listenThread.detach();
	}
	else 
	{
		SendMessage(_replySocket, "INVALID_STATE");
	}
}

void CefAdapterInterProcessCommunicator::FailedToInitializeApplication()
{
	Invoke("FAILED_TO_INITIALIZE_APPLICATION");	
}

void CefAdapterInterProcessCommunicator::Shutdown()
{
	Invoke("SHUTDOWN");	
}

void CefAdapterInterProcessCommunicator::OnBrowserCreated(int browserId)
{
	std::cout << "CefAdapterInterProcessCommunicator::OnBrowserCreated(" << browserId << ")" << std::endl;

	std::ostringstream message;

	message << "ON_BROWSER_CREATED|" << browserId;

	Invoke(message.str());
}

void CefAdapterInterProcessCommunicator::OnBrowserClosed(int browserId)
{
	std::cout << "CefAdapterInterProcessCommunicator::OnBrowserClosed(" << browserId << ")" << std::endl;

	std::ostringstream message;

	message << "ON_BROWSER_CLOSED|" << browserId;

	Invoke(message.str());
}

void CefAdapterInterProcessCommunicator::OnContextCreated(int browserId, long frameId)
{
	std::cout << "CefAdapterInterProcessCommunicator::OnContextCreated(" << browserId << ", " << frameId << ")" << std::endl;

	std::ostringstream message;

	message << "ON_CONTEXT_CREATED|" << browserId << "|" << frameId;

	Invoke(message.str());
}

bool CefAdapterInterProcessCommunicator::OnQuery(int browserId, long frameId, long queryId, std::string query)
{
	std::cout << "CefAdapterInterProcessCommunicator::OnQuery(" << browserId << ", " << frameId << ", " << queryId << ", " << query << ")" << std::endl;

	std::ostringstream message;

	message << "ON_QUERY|" << browserId << "|" << frameId << "|" << queryId << "|" << query;

	auto result = Invoke(message.str());

	return result.compare("ON_QUERY|SUCCESS|") == 0;
}

void CefAdapterInterProcessCommunicator::ListenRequests()
{
	while (_replySocket->connected())
	{
		std::string message = ReceiveMessage(_replySocket);

		const std::string SHOW_DEVELOPER_TOOLS = "SHOW_DEVELOPER_TOOLS";
		const std::string QUERY_SUCCESS = "QUERY_SUCCESS";
		const std::string QUERY_FAILURE = "QUERY_FAILURE";
		const std::string EXECUTE_JAVA_SCRIPT = "EXECUTE_JAVA_SCRIPT";

		auto splittedMessage = SplitString(message, '|');

		auto requestName = splittedMessage[0];

		if (requestName.compare(SHOW_DEVELOPER_TOOLS) == 0)
		{
			int browserId;

			if (std::sscanf(splittedMessage[1].c_str(), "%d", &browserId) == 1)
			{
				if (_showDeveloperToolsCallback(browserId))
				{
					SendMessage(_replySocket, "SHOW_DEVELOPER_TOOLS|SUCCESS");
				}
				else
				{
					SendMessage(_replySocket, "SHOW_DEVELOPER_TOOLS|FAILURE");
				}
			}
			else
			{
				SendMessage(_replySocket, "SHOW_DEVELOPER_TOOLS|INVALID_ARGUMENTS");
			}
		}
		else if (requestName.compare(QUERY_SUCCESS) == 0)
		{
			SendMessage(_replySocket, "QUERY_SUCCESS|SUCCESS");

			long queryId;

			std::sscanf(splittedMessage[1].c_str(), "%ld", &queryId);

			_querySuccessCallback(queryId, splittedMessage[2]);			
		}
		else if (requestName.compare(QUERY_FAILURE) == 0)
		{
			SendMessage(_replySocket, "QUERY_FAILURE|SUCCESS");

			long queryId;
			int errorCode;

			std::sscanf(splittedMessage[1].c_str(), "%ld", &queryId);
			std::sscanf(splittedMessage[2].c_str(), "%d", &errorCode);

			_queryFailureCallback(queryId, errorCode, splittedMessage[3]);			
		}
		else if (requestName.compare(EXECUTE_JAVA_SCRIPT) == 0)
		{
			int browserId;

			std::sscanf(splittedMessage[1].c_str(), "%d", &browserId);

			bool result = _executeJavaScriptCallback(browserId, splittedMessage[2]);

			if (result)
			{
				SendMessage(_replySocket, "EXECUTE_JAVA_SCRIPT|SUCCESS");
			}
			else
			{
				SendMessage(_replySocket, "EXECUTE_JAVA_SCRIPT|FAILURE");
			}
		}
		else
		{
			SendMessage(_replySocket, "INVALID_REQUEST");
		}
	}
}

std::string CefAdapterInterProcessCommunicator::Invoke(std::string request)
{
	SendMessage(_requestSocket, request);
	return ReceiveMessage(_requestSocket);
}