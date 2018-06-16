#pragma once

#include "include/cef_app.h"

enum JavaScriptType
{
	Undefined = 0,
	Number,
	String,
	Boolean,
	Void 
};

struct JavaScriptValue
{
	JavaScriptType ValueType;
	bool BooleanValue;
	double NumberValue;
    const char* StringValue;
};

typedef VOID (CALLBACK* ProgressCallback)(INT32, LPCTSTR);
typedef VOID (CALLBACK* ErrorCallback)(INT32, LPCTSTR);
typedef VOID (CALLBACK* BrowserCreatedCallback)(INT32);
typedef VOID (CALLBACK* ContextCreatedCallback)(INT32, INT32);
typedef VOID (CALLBACK* ExecuteJsFunctionCallback)(INT32, const char*, INT32, JavaScriptValue*);
typedef VOID (CALLBACK* QuerySuccessCallback)(const char*);
typedef VOID (CALLBACK* QueryFailureCallback)(INT32, const char*);
typedef bool (CALLBACK* QueryCallback)(INT32, INT32, INT64, const char*, QuerySuccessCallback, QueryFailureCallback);

class CefAdapterBrowserApplication : public CefApp, public CefBrowserProcessHandler
{
public:
	CefAdapterBrowserApplication(std::string url, BrowserCreatedCallback browserCreatedCallback, 
		ContextCreatedCallback contextCreatedCallback, ExecuteJsFunctionCallback executeJsFunctionCallback);

	~CefAdapterBrowserApplication();

	// Provide access to the single global instance of this object.
	static CefAdapterBrowserApplication* GetInstance();

	// CefApp methods:
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE 
	{
		return this;
	}

	// CefBrowserProcessHandler methods:
	virtual void OnContextInitialized() OVERRIDE;	

	CefRefPtr<CefBrowser> GetBrowser();

private:
	IMPLEMENT_REFCOUNTING(CefAdapterBrowserApplication);

	std::string _url;
	BrowserCreatedCallback _browserCreatedCallback;	
	ContextCreatedCallback _contextCreatedCallback;
	ExecuteJsFunctionCallback _executeJsFunctionCallback;
	CefRefPtr<CefBrowser> _browser;

};

