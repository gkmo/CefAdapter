#include <iostream>
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_helpers.h"

#include "CefAdapterRendererApplication.h"

namespace
{
	CefAdapterRendererApplication* g_ApplicationInstance = NULL;
}

CefAdapterRendererApplication::CefAdapterRendererApplication(Logger* logger)
{
	_logger = logger;
	g_ApplicationInstance = this;
}

CefAdapterRendererApplication::~CefAdapterRendererApplication()
{
	g_ApplicationInstance = NULL;
}

void CefAdapterRendererApplication::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
	_logger->Info("CefAdapterRendererApplication", "OnContextCreated");

	CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("OnContextCreated");

	CefRefPtr<CefListValue> args = msg->GetArgumentList();

	args->SetInt(0, frame->GetIdentifier());

	browser->SendProcessMessage(PID_BROWSER, msg);	

	_messageRouter->OnContextCreated(browser, frame, context);
}

void CefAdapterRendererApplication::OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
    _messageRouter->OnContextReleased(browser, frame, context);
}

void CefAdapterRendererApplication::OnWebKitInitialized()
{
	_logger->Info("CefAdapterRendererApplication", "OnWebKitInitialized");

	CefMessageRouterConfig config;	 
	config.js_query_function = "executeQuery";
	config.js_cancel_function = "cancelQuery";

    _messageRouter = CefMessageRouterRendererSide::Create(config);

	std::string extensionCode =
	"var ipcDotNet;"
    "if (!ipcDotNet)"
    "  ipcDotNet = {};"
    "(function() {"
	"  ipcDotNet.callbacks = new Map();"
    "  ipcDotNet.send = function(channel, argument, onSuccess, onFailure) {"
	"    let request = { channel: channel, argument: argument };"
    "    window.executeQuery({"
    "      request: JSON.stringify(request),"
    "      onSuccess: onSuccess,"
    "      onFailure: onFailure"
    "    });"
    "  };"
	"  ipcDotNet.on = function(channel, callback) { this.callbacks.set(channel, callback); };"
    "  ipcDotNet.receive = function(channel, argument) {"
    "    let callback = this.callbacks.get(channel);"
    "    if (callback) { callback(this, argument); }"
    "  };"
    "})();";

  	// Register the extension.
	CefRegisterExtension("v8/test", extensionCode, NULL);
}

bool CefAdapterRendererApplication::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId sourceProcess, CefRefPtr<CefProcessMessage> message)
{	
	const std::string& messageName = message->GetName();

	std::ostringstream stringStream;
	
	stringStream << "OnProcessMessageReceived. Source Process Id = " << sourceProcess << "; Browser Id = " << browser->GetIdentifier() << "; Message Name = " << messageName;	

	_logger->Info("CefAdapterRendererApplication", stringStream.str().c_str());

	return _messageRouter->OnProcessMessageReceived(browser, sourceProcess, message);;
}