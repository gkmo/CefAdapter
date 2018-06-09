#include "CefAdapterExtensionHandler.h"

bool CefAdapterExtensionHandler::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments,
	CefRefPtr<CefV8Value>& retval, CefString& exception)
{
	const char* nameArg = name.ToString().c_str();

	_logger->Debug("CefAdapterExtensionHandler", nameArg);

	CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create("ExecuteJsFunction");

	CefRefPtr<CefListValue> messageArguments = message->GetArgumentList();


	CefRefPtr<CefBinaryValue> opa = CefBinaryValue::Create(nameArg, strlen(nameArg));

	messageArguments->SetInt(0, 10);
	//messageArguments->SetString(1, "mycsharpfunc");
	messageArguments->SetBinary(1, opa);

	/*int index = 2;

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
	}*/

	messageArguments->SetBool(2, true);

	_browser->SendProcessMessage(PID_BROWSER, message);

	return true;
}