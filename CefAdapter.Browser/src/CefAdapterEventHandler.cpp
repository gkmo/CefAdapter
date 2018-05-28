#include "stdafx.h"
#include <sstream>
#include <string>

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

CefAdapterEventHandler::CefAdapterEventHandler(BrowserCreatedCallback browserCreatedCallback) : _isClosing(false)
{
	DCHECK(!g_instance);
	g_instance = this;

	_browserCreatedCallback = browserCreatedCallback;
}


CefAdapterEventHandler::~CefAdapterEventHandler()
{
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

void CefAdapterEventHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) 
{
	CEF_REQUIRE_UI_THREAD();

	// Add to the list of existing browsers.
	_browserList.push_back(browser);	

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
		// All browser windows have closed. Quit the application message loop.
		CefQuitMessageLoop();
	}
}

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
