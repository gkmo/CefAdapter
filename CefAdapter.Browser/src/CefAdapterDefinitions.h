#pragma once


#if defined(OS_WIN)	
#define EXPORT __declspec(dllexport)
#else 
#define EXPORT __attribute__((visibility("default")))
#endif

enum JavaScriptType
{
	Boolean = 0,	
	Number,
	String,
	Undefined
};

struct JavaScriptValue
{
	JavaScriptType ValueType;
	bool BooleanValue;
	double NumberValue;
	const char* StringValue;
};

typedef void (*ErrorCallback)(int, const char*);
typedef void (*BrowserCreatedCallback)(int);
typedef void (*ContextCreatedCallback)(int, int);
typedef JavaScriptValue (*ExecuteJsFunctionCallback)(int, const char*, int, JavaScriptValue*);
typedef void (*QuerySuccessCallback)(const char*);
typedef void (*QueryFailureCallback)(int, const char*);
typedef bool (*QueryCallback)(int, int, long, const char*, QuerySuccessCallback, QueryFailureCallback);
