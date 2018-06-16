#pragma once


#if defined(OS_WIN)	
#define EXPORT __declspec(dllexport)
#else 
#define EXPORT __attribute__((visibility("default")))
#endif

enum CefAdapterValueType
{
	Boolean = 0,
	Byte,
	Int16,
	Int32,
	Int64,
	Double,
	String
};

struct CefAdapterValue
{
	CefAdapterValueType ValueType;
	void* Value;
};

typedef void (*ErrorCallback)(int, const char*);
typedef void (*BrowserCreatedCallback)(int);
typedef void (*ContextCreatedCallback)(int, int);
typedef CefAdapterValue (*ExecuteJsFunctionCallback)(int, const char*, int, CefAdapterValue*);