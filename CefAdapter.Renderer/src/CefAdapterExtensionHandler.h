#pragma once

#include "include/cef_app.h"

class CefAdapterExtensionHandler : public CefV8Handler {
public:
  CefAdapterExtensionHandler() {}

  virtual bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments,
                       CefRefPtr<CefV8Value>& retval, CefString& exception) OVERRIDE;

  // Provide the reference counting implementation for this class.
  IMPLEMENT_REFCOUNTING(CefAdapterExtensionHandler);
};