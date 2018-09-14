#include <thread>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>

#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_helpers.h"
#include "include/base/cef_bind.h"
#include "include/wrapper/cef_closure_task.h"

#include "CefAdapterBrowserApplication.h"
#include "CefAdapterEventHandler.h"

namespace
{
	CefAdapterBrowserApplication* g_ApplicationInstance = NULL;
}

CefAdapterBrowserApplication::CefAdapterBrowserApplication(Logger* logger)
{	
	_eventHandler = new CefAdapterEventHandler();
	_ipc = new CefAdapterInterProcessCommunicator(logger);

	auto browserCreatedCallback = std::bind<void>(&CefAdapterInterProcessCommunicator::OnBrowserCreated, _ipc, std::placeholders::_1);
	auto browserClosedCallback = std::bind<void>(&CefAdapterInterProcessCommunicator::OnBrowserClosed, _ipc, std::placeholders::_1);
	auto contextCreatedCallback = std::bind<void>(&CefAdapterInterProcessCommunicator::OnContextCreated, _ipc, std::placeholders::_1, std::placeholders::_2);
	auto queryCallback = std::bind<bool>(&CefAdapterInterProcessCommunicator::OnQuery, _ipc, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

	_eventHandler->SetBrowserCreatedCallback(browserCreatedCallback);
	_eventHandler->SetBrowserClosedCallback(browserClosedCallback);
	_eventHandler->SetContextCreatedCallback(contextCreatedCallback);
	_eventHandler->SetQueryCallback(queryCallback);

	auto executeJavaScriptCallback = std::bind<bool>(&CefAdapterBrowserApplication::ExecuteJavaScript, this, std::placeholders::_1, std::placeholders::_2);
	auto showDeveloperToolsCallback = std::bind<void>(&CefAdapterBrowserApplication::ShowDeveloperTools, this, std::placeholders::_1);
	auto onQuerySuccessCallback = std::bind<void>(&CefAdapterEventHandler::OnQuerySuccess, _eventHandler, std::placeholders::_1, std::placeholders::_2);
	auto onQueryFailureCallback = std::bind<void>(&CefAdapterEventHandler::OnQueryFailure, _eventHandler, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	auto setWindowIconCallback = std::bind<bool>(&CefAdapterBrowserApplication::SetWindowIcon, this, std::placeholders::_1, std::placeholders::_2);
	auto setWindowTitleCallback = std::bind<bool>(&CefAdapterBrowserApplication::SetWindowTitle, this, std::placeholders::_1, std::placeholders::_2);

	_ipc->SetExecuteJavaScriptCallback(executeJavaScriptCallback);
	_ipc->SetShowDeveloperToolsCallback(showDeveloperToolsCallback);
	_ipc->SetQuerySuccessCallback(onQuerySuccessCallback);
	_ipc->SetQueryFailureCallback(onQueryFailureCallback);
	_ipc->SetWindowIconCallback(setWindowIconCallback);
	_ipc->SetWindowTitleCallback(setWindowTitleCallback);	

    logger->Info("Initialization", "Waiting external connection...");

    _ipc->WaitConnection();

	g_ApplicationInstance = this;
}

CefAdapterBrowserApplication::~CefAdapterBrowserApplication()
{
	_ipc->Shutdown();

    delete _ipc;

	g_ApplicationInstance = NULL;
}

CefAdapterBrowserApplication * CefAdapterBrowserApplication::GetInstance()
{
	return g_ApplicationInstance;
}

void CefAdapterBrowserApplication::OnContextInitialized()
{
	CEF_REQUIRE_UI_THREAD();

	std::cout << "CefAdapterBrowserApplication::OnContextInitialized()" << std::endl;	

	// Specify CEF browser settings here.
	CefBrowserSettings browserSettings;	

	CefRefPtr<CefCommandLine> commandLine = CefCommandLine::GetGlobalCommandLine();
    
    std::string url = commandLine->GetSwitchValue("url");
    if (url.empty())
    {
        url = "http://www.google.com";
    }

	// Information used when creating the native window.
	CefWindowInfo windowInfo;	

#if defined(OS_WIN)
	// On Windows we need to specify certain flags that will be passed to
	// CreateWindowEx().
	windowInfo.SetAsPopup(NULL, "CefAdapter");
#endif

	int width = 800;
	int height = 600;

	if (commandLine->HasSwitch("width"))
	{
		std::stringstream ss(commandLine->GetSwitchValue("width"));
		ss >> width;
	}

	if (commandLine->HasSwitch("height"))
	{
		std::stringstream ss(commandLine->GetSwitchValue("height"));
		ss >> height;
	}

	windowInfo.width = width;
	windowInfo.height = height;

	// Create the first browser window.
	CefBrowserHost::CreateBrowser(windowInfo, _eventHandler, url, browserSettings, NULL);	
}

CefRefPtr<CefBrowser> CefAdapterBrowserApplication::GetMainBrowser()
{	
	return _mainBrowser;
}

CefRefPtr<CefAdapterEventHandler> CefAdapterBrowserApplication::GetEventHandler()
{
	return _eventHandler;
}

void CefAdapterBrowserApplication::ShowDeveloperTools(int browserId)
{
	if (!CefCurrentlyOn(TID_UI))
	{
		CefPostTask(TID_UI, base::Bind(&CefAdapterBrowserApplication::ShowDeveloperTools, this, browserId));
		return;
	}

	auto eventHandler = CefAdapterBrowserApplication::GetInstance()->GetEventHandler();

	CefRefPtr<CefBrowser> browser = eventHandler->GetBrowserById(browserId);

	if (browser)
	{
		// Specify CEF browser settings here.
		CefBrowserSettings browserSettings;

		// Information used when creating the native window.
		CefWindowInfo windowInfo;		

#if defined(OS_WIN)
		// On Windows we need to specify certain flags that will be passed to
		// CreateWindowEx().
		windowInfo.SetAsPopup(NULL, "CefAdapter.DeveloperTools");
#endif

		windowInfo.width = 800;
		windowInfo.height = 600;

		CefRefPtr<CefClient> client(eventHandler);

		CefPoint point;

		browser->GetHost()->ShowDevTools(windowInfo, client.get(), browserSettings, point);		
	}	
}

bool CefAdapterBrowserApplication::ExecuteJavaScript(int browserId, std::string code)
{
	CefRefPtr<CefBrowser> browser = _eventHandler->GetBrowserById(browserId);

	if (browser)
	{
		CefRefPtr<CefFrame> mainFrame = browser->GetMainFrame();

		mainFrame->ExecuteJavaScript(code, mainFrame->GetURL(), 0);

		return true;
	}

	return false;
}

bool CefAdapterBrowserApplication::SetWindowIcon(int browserId, std::string iconPath)
{
	CefRefPtr<CefBrowser> browser = _eventHandler->GetBrowserById(browserId);

	if (browser)
	{	
#if defined(OS_WIN)	
		CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();

		std::wstring stricon = std::wstring(iconPath.begin(), iconPath.end());
			
		auto hWindowIcon =(HICON)LoadImage(NULL, stricon.c_str(), IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
		auto hWindowIconBig =(HICON)LoadImage(NULL, stricon.c_str(), IMAGE_ICON, 48, 48, LR_LOADFROMFILE);

		SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hWindowIconBig);
		SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hWindowIcon);

		return true;
#endif
	}

	return false;
}

bool CefAdapterBrowserApplication::SetWindowTitle(int browserId, std::string title)
{
	CefRefPtr<CefBrowser> browser = _eventHandler->GetBrowserById(browserId);

	if (browser)
	{
#if defined(OS_WIN)	
		CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();

		std::wstring wtitle(title.begin(), title.end());

		SetWindowText(hwnd, wtitle.c_str());

		return true;
#endif
	}

	return false;
}