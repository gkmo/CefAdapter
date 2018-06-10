#include "CefAdapterExtensionHandler.h"

bool CefAdapterExtensionHandler::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments,
	CefRefPtr<CefV8Value>& retval, CefString& exception)
{
	const char* nameArg = name.ToString().c_str();

	_logger->Debug("CefAdapterExtensionHandler", nameArg);

	CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create("ExecuteJsFunction");

	CefRefPtr<CefListValue> messageArguments = message->GetArgumentList();

	messageArguments->SetString(0, name);
	
	int index = 2;

	for (auto argument : arguments)
	{
		if (argument->IsBool())
		{
			messageArguments->SetBool(index++, argument->GetBoolValue());
		}
		else if (argument->IsInt())
		{
			messageArguments->SetInt(index++, argument->GetIntValue());
		}
		else if (argument->IsString())
		{
			messageArguments->SetString(index++, argument->GetStringValue());
		}
		else if (argument->IsDouble())
		{
			messageArguments->SetDouble(index++, argument->GetDoubleValue());
		}
		else if (argument->IsNull())
		{
			messageArguments->SetNull(index++);
		}
	}

	messageArguments->SetInt(1, index - 2);
	
	_browser->SendProcessMessage(PID_BROWSER, message);

	return true;
}