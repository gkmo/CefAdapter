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

CefAdapterRendererApplication::CefAdapterRendererApplication()
{
	g_ApplicationInstance = this;
}

CefAdapterRendererApplication::~CefAdapterRendererApplication()
{
	g_ApplicationInstance = NULL;
}

void CefAdapterRendererApplication::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{	
	std::cout << "OnContextCreated" << std::endl;

	// Create the message object.
	CefRefPtr<CefProcessMessage> msg= CefProcessMessage::Create("OnContextCreated");

	// Retrieve the argument list object.
	CefRefPtr<CefListValue> args = msg->GetArgumentList();

	// Populate the argument values.
	args->SetInt(0, browser->GetIdentifier());
	args->SetInt(1, frame->GetIdentifier());

	// Send the process message to the render process.
	// Use PID_BROWSER instead when sending a message to the browser process.
	browser->SendProcessMessage(PID_BROWSER, msg);

	// Retrieve the context's window object.
	CefRefPtr<CefV8Value> object = context->GetGlobal();

	// Create a new V8 string value. See the "Basic JS Types" section below.
	CefRefPtr<CefV8Value> str = CefV8Value::CreateString("My Value!");

	// Add the string to the window object as "window.myval". See the "JS Objects" section below.
	object->SetValue("myval", str, V8_PROPERTY_ATTRIBUTE_NONE);	
}

void CefAdapterRendererApplication::OnWebKitInitialized() 
{
	// Define the extension contents.
  std::string extensionCode =
    "var test;"
    "if (!test)"
    "  test = {};"
    "(function() {"
    "  test.myfunc = function() {"
    "    native function myfunc();"
    "    return myfunc();"
    "  };"
    "})();";

  // Create an instance of my CefV8Handler object.
  CefRefPtr<CefV8Handler> handler = new CefAdapterExtensionHandler();

  // Register the extension.
  CefRegisterExtension("v8/test", extensionCode, handler);
}

bool CefAdapterRendererApplication::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) 
{
  // Check the message name.
  const std::string& message_name = message->GetName();

  if (message_name == "my_message") 
  {
    // Handle the message here...
    return true;
  }
  
  return false;
}