#include <iostream>
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_helpers.h"

#include "CefAdapterRendererApplication.h"
#include "CefAdapterExtensionHandler.h"

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
	_logger->Debug("CefAdapterRendererApplication", "OnContextCreated");

	CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("OnContextCreated");

	CefRefPtr<CefListValue> args = msg->GetArgumentList();

	args->SetInt(0, frame->GetIdentifier());

	browser->SendProcessMessage(PID_BROWSER, msg);	

	//// Retrieve the context's window object.
	//CefRefPtr<CefV8Value> object = context->GetGlobal();

	//// Create a new V8 string value. See the "Basic JS Types" section below.
	//CefRefPtr<CefV8Value> str = CefV8Value::CreateString("My Value!");

	//// Add the string to the window object as "window.myval". See the "JS Objects" section below.
	//object->SetValue("myval", str, V8_PROPERTY_ATTRIBUTE_NONE);

	_messageRouter->OnContextCreated(browser, frame, context);
}

void CefAdapterRendererApplication::OnWebKitInitialized()
{
	_logger->Debug("CefAdapterRendererApplication", "OnWebKitInitialized");

	CefMessageRouterConfig config;
    _messageRouter = CefMessageRouterRendererSide::Create(config);

	// Define the extension contents.
	/*std::string extensionCode =
		"var test;"
		"if (!test)"
		"  test = {};"
		"(function() {"
		"  test.myfunc = function() {"
		"    native function myfunc();"
		"    return myfunc();"
		"  };"
		"})();";*/

	//// Create an instance of my CefV8Handler object.
	//CefRefPtr<CefV8Handler> handler = new CefAdapterExtensionHandler();

	//// Register the extension.
	//CefRegisterExtension("v8/test", extensionCode, handler);	

	//CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("OnWebKitInitialized");

	//CefRefPtr<CefListValue> args = msg->GetArgumentList();

	//args->SetInt(0, frame->GetIdentifier());

	//browser->SendProcessMessage(PID_BROWSER, msg);
}

bool CefAdapterRendererApplication::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId sourceProcess, CefRefPtr<CefProcessMessage> message)
{
	// Check the message name.
	const std::string& messageName = message->GetName();

	std::ostringstream stringStream;
	
	stringStream << "OnProcessMessageReceived. Source Process Id = " << sourceProcess << "; Browser Id = " << browser->GetIdentifier() << "; Message Name = " << messageName;	

	_logger->Debug("CefAdapterRendererApplication", stringStream.str().c_str());

	if (messageName == "CreateJsGlobalFunction")
	{
		auto arguments = message->GetArgumentList();

		auto functionName = arguments->GetString(0);

		CefRefPtr<CefV8Context> context = browser->GetMainFrame()->GetV8Context();

		if (context->Enter())
		{
			// Retrieve the context's window object.
			CefRefPtr<CefV8Value> global = context->GetGlobal();

			// Create an instance of my CefV8Handler object.
			CefRefPtr<CefV8Handler> handler = new CefAdapterExtensionHandler(browser, _logger);

			// Create the "myfunc" function.
			CefRefPtr<CefV8Value> function = CefV8Value::CreateFunction(functionName, handler);

			// Add the function to the "window" object.
			global->SetValue(functionName, function.get(), V8_PROPERTY_ATTRIBUTE_NONE);

			context->Exit();
		}

		return true;
	}

	return false;
}