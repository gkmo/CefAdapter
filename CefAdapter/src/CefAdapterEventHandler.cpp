#include <sstream>
#include <string>
#include <iostream>
#include <functional>

#include "include/base/cef_bind.h"
#include "include/cef_app.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"

#include "CefAdapterEventHandler.h"

CefAdapterEventHandler::CefAdapterEventHandler()
{
	_messageHandler = new CefAdapterMessageHandler();
	_isClosing = false;
}

CefAdapterEventHandler::~CefAdapterEventHandler()
{
	delete _messageHandler;
	_messageHandler = NULL;
}

void CefAdapterEventHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
	CEF_REQUIRE_UI_THREAD();

	// Set the title of the window using platform APIs.
	PlatformTitleChange(browser, title);
}

void CefAdapterEventHandler::PlatformTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
#if defined(OS_WIN)
	// CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
	// SetWindowText(hwnd, std::wstring(title).c_str());
#endif
}

bool CefAdapterEventHandler::OnPreKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent& event, CefEventHandle os_event, bool* is_keyboard_shortcut) 
{
	if (browser->GetIdentifier() != 1)
	{
		return false;
	}

	//std::cout << "OnPreKeyEvent: " << os_event->message << " " << event.windows_key_code << std::endl;

	// if (os_event && os_event->message == WM_KEYDOWN) 
	// {
	// 	switch (event.windows_key_code)
	// 	{
	// 		case VK_F12: 
	// 		{
	// 			// Specify CEF browser settings here.
	// 			CefBrowserSettings browserSettings;

	// 			// Information used when creating the native window.
	// 			CefWindowInfo windowInfo;
	// 			windowInfo.width = 400;
	// 			windowInfo.height = 400;

	// #if defined(OS_WIN)
	// 			// On Windows we need to specify certain flags that will be passed to
	// 			// CreateWindowEx().
	// 			windowInfo.SetAsPopup(NULL, "CefAdapter.DeveloperTools");
	// #endif
	// 			CefRefPtr<CefClient> client(CefAdapterEventHandler::GetInstance());

	// 			CefPoint point;

	// 			browser->GetHost()->ShowDevTools(windowInfo, client.get(), browserSettings, point);

	// 			return true;
	// 		};
	// 		break;
	// 	}      
	// }

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
		config.js_query_function = "executeQuery";
		config.js_cancel_function = "cancelQuery";
		
		_messageRouter = CefMessageRouterBrowserSide::Create(config);
		_messageRouter->AddHandler(_messageHandler, false);
  	}

	if (_browserCreatedCallback != NULL)
	{
		_browserCreatedCallback(browser->GetIdentifier());
	}
}

bool CefAdapterEventHandler::DoClose(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();

	if (_browserClosedCallback != NULL)
	{
		_browserClosedCallback(browser->GetIdentifier());
	}

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

		if (_contextCreatedCallback != NULL)
		{
			_contextCreatedCallback(browserId, frameId);
		}

		return true;
	}	

	return _messageRouter->OnProcessMessageReceived(browser, source_process, message);
}

void CefAdapterEventHandler::SetQueryCallback(std::function<bool(int, long, long, std::string)> callback)
{
	_messageHandler->SetQueryCallback(callback);
}

void CefAdapterEventHandler::SetBrowserCreatedCallback(std::function<void(int)> listener)
{
	_browserCreatedCallback = listener;
}

void CefAdapterEventHandler::SetBrowserClosedCallback(std::function<void(int)> listener)
{
	_browserClosedCallback = listener;
}

void CefAdapterEventHandler::SetContextCreatedCallback(std::function<void(int,int)> listener)
{
	_contextCreatedCallback = listener;
}

void CefAdapterEventHandler::OnQuerySuccess(long queryId, std::string result)
{
	_messageHandler->OnSuccess(queryId, result);
}

void CefAdapterEventHandler::OnQueryFailure(long queryId, int errorCode, std::string result)
{
	_messageHandler->OnFailure(queryId, errorCode, result);
}