#include "CefAdapterExtensionHandler.h"

bool CefAdapterExtensionHandler::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments,
                       CefRefPtr<CefV8Value>& retval, CefString& exception) 
{
    if (name == "myfunc") 
    {
        // Return my string value.
        retval = CefV8Value::CreateString("My Value!");
        return true;
    }

    // Function does not exist.
    return false;
}