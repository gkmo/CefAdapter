#include <sstream>
#include <string>
#include <iostream>

#include "include/base/cef_bind.h"
#include "include/cef_app.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"

#include "CefAdapterEventHandler.h"

namespace
{
	CefAdapterEventHandler* g_instance = NULL;
}

CefAdapterEventHandler::CefAdapterEventHandler(BrowserCreatedCallback browserCreatedCallback, 
	ContextCreatedCallback contextCreatedCallback, ExecuteJsFunctionCallback executeJsFunctionCallback)
{
	DCHECK(!g_instance);

	_browserCreatedCallback = browserCreatedCallback;
	_contextCreatedCallback = contextCreatedCallback;
	_executeJsFunctionCallback = executeJsFunctionCallback;
	_messageHandler = new CefAdapterMessageHandler();
	_isClosing = false;
	g_instance = this;
}


CefAdapterEventHandler::~CefAdapterEventHandler()
{
	delete _messageHandler;
	_messageHandler = NULL;
	g_instance = NULL;
}

CefAdapterEventHandler * CefAdapterEventHandler::GetInstance()
{
	return g_instance;
}

void CefAdapterEventHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
	CEF_REQUIRE_UI_THREAD();

	// Set the title of the window using platform APIs.
	PlatformTitleChange(browser, title);
}

void CefAdapterEventHandler::PlatformTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
	CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
	SetWindowText(hwnd, std::wstring(title).c_str());
}

bool CefAdapterEventHandler::OnPreKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent& event, CefEventHandle os_event, bool* is_keyboard_shortcut) 
{
	if (browser->GetIdentifier() != 1)
	{
		return false;
	}

	std::cout << "OnPreKeyEvent: " << os_event->message << " " << event.windows_key_code << std::endl;

	if (os_event && os_event->message == WM_KEYDOWN) 
	{
		switch (event.windows_key_code)
		{
			case VK_F12: 
			{
				// Specify CEF browser settings here.
				CefBrowserSettings browserSettings;

				// Information used when creating the native window.
				CefWindowInfo windowInfo;
				windowInfo.width = 400;
				windowInfo.height = 400;

	#if defined(OS_WIN)
				// On Windows we need to specify certain flags that will be passed to
				// CreateWindowEx().
				windowInfo.SetAsPopup(NULL, "CefAdapter.DeveloperTools");
	#endif
				CefRefPtr<CefClient> client(CefAdapterEventHandler::GetInstance());

				CefPoint point;

				browser->GetHost()->ShowDevTools(windowInfo, client.get(), browserSettings, point);

				return true;
			};
			break;
		}      
	}

	return false;
}

void CefAdapterEventHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();

	// Add to the list of existing browsers.
	_browserList.push_back(browser);

	if (!_messageRouter) 
	{
		// Create the browser-side router for query handling.
		CefMessageRouterConfig config;
		_messageRouter = CefMessageRouterBrowserSide::Create(config);
		_messageRouter->AddHandler(_messageHandler, false);
  	}

	_browserCreatedCallback(browser->GetIdentifier());
}

bool CefAdapterEventHandler::DoClose(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();

	// Closing the main window requires special handling. See the DoClose()
	// documentation in the CEF header for a detailed destription of this
	// process.
	if (_browserList.size() == 1)
	{
		// Set a flag to indicate that the window close should be allowed.
		_isClosing = true;
	}

	// Allow the close. For windowed browsers this will result in the OS close
	// event being sent.
	return false;
}

void CefAdapterEventHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();

	// Remove from the list of existing browsers.
	BrowserList::iterator bit = _browserList.begin();

	for (; bit != _browserList.end(); ++bit)
	{
		if ((*bit)->IsSame(browser))
		{
			_browserList.erase(bit);
			break;
		}
	}

	if (_browserList.empty())
	{		
		 _messageRouter->RemoveHandler(_messageHandler);
		 _messageRouter = NULL;

		// All browser windows have closed. Quit the application message loop.
		CefQuitMessageLoop();
	}
}

// bool CefAdapterEventHandler::OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
//                                    CefRefPtr<CefFrame> frame,
//                                    CefRefPtr<CefRequest> request,
//                                    bool user_gesture,
//                                    bool is_redirect) {
//   CEF_REQUIRE_UI_THREAD();

//   _messageRouter->OnBeforeBrowse(browser, frame);
//   return false;
// }

void CefAdapterEventHandler::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
	ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl)
{
	CEF_REQUIRE_UI_THREAD();

	// Don't display an error for downloaded files.
	if (errorCode == ERR_ABORTED)
	{
		return;
	}

	// Display a load error message.
	std::stringstream ss;

	ss << "<html><body bgcolor=\"white\">"
		"<h2>Failed to load URL "
		<< std::string(failedUrl) << " with error " << std::string(errorText)
		<< " (" << errorCode << ").</h2></body></html>";

	frame->LoadString(ss.str(), failedUrl);
}

void CefAdapterEventHandler::CloseAllBrowsers(bool forceClose)
{
	if (!CefCurrentlyOn(TID_UI))
	{
		// Execute on the UI thread.
		CefPostTask(TID_UI, base::Bind(&CefAdapterEventHandler::CloseAllBrowsers, this, forceClose));

		return;
	}

	if (_browserList.empty())
	{
		return;
	}

	BrowserList::const_iterator it = _browserList.begin();

	for (; it != _browserList.end(); ++it)
	{
		(*it)->GetHost()->CloseBrowser(forceClose);
	}
}

CefRefPtr<CefBrowser> CefAdapterEventHandler::GetBrowserById(int id)
{
	BrowserList::const_iterator it = _browserList.begin();

	for (; it != _browserList.end(); ++it)
	{
		if ((*it)->GetIdentifier() == id)
		{
			return (*it);
		}
	}

	return NULL;
}

bool CefAdapterEventHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{	
	const std::string& messageName = message->GetName();

	std::cout << "CefAdapterEventHandler::OnProcessMessageReceived: " << messageName << std::endl;		

	if (messageName == "OnContextCreated")
	{
		CefRefPtr<CefListValue> args = message->GetArgumentList();

		int browserId = browser->GetIdentifier();
		int frameId = args->GetInt(0);

		std::cout << "OnContextCreated -> Browser ID = " << browserId << "; Frame ID = " << frameId << std::endl;

		_contextCreatedCallback(browserId, frameId);

		return true;
	}
	else if (messageName == "ExecuteJsFunction")
	{
		CefRefPtr<CefListValue> args = message->GetArgumentList();

		auto functionName = args->GetString(0).ToString();		
		auto numberOfArguments = args->GetInt(1);

		std::cout << "ExecuteJsFunction -> Name = " << functionName << "; Arguments = " << numberOfArguments << std::endl;

		JavaScriptValue* convertedArguments = new JavaScriptValue[numberOfArguments];

		ConvertToJavaScriptValues(numberOfArguments, args, convertedArguments);

		_executeJsFunctionCallback(browser->GetIdentifier(), functionName.c_str() , 0, convertedArguments);

		delete convertedArguments;

		return true;
	}

	return _messageRouter->OnProcessMessageReceived(browser, source_process, message);
}

void CefAdapterEventHandler::ConvertToJavaScriptValues(int numberOfArguments, CefRefPtr<CefListValue> args, JavaScriptValue* convertedValues)
{	
	const int offset = 2;

	for(size_t i = 0; i < numberOfArguments; i++)
	{
		auto argumentType = args->GetType(i + offset);

		if (argumentType == CefValueType::VTYPE_INT)
		{
			convertedValues[i].ValueType = JavaScriptType::Number;
			convertedValues[i].NumberValue = args->GetInt(i + offset);
		}
		else if (argumentType == CefValueType::VTYPE_DOUBLE)
		{
			convertedValues[i].ValueType = JavaScriptType::Number;
			convertedValues[i].NumberValue = args->GetDouble(i + offset);
		}
		else if (argumentType == CefValueType::VTYPE_BOOL)
		{
			convertedValues[i].ValueType = JavaScriptType::Boolean;
			convertedValues[i].BooleanValue = args->GetBool(i + offset);
		}
		else if (argumentType == CefValueType::VTYPE_STRING)
		{
			convertedValues[i].ValueType = JavaScriptType::String;
			convertedValues[i].StringValue = args->GetString(i + offset).ToString().c_str();
		}
	}
	
}