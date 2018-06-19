#pragma once


#if defined(OS_WIN)	
#define EXPORT __declspec(dllexport)
#else 
#define EXPORT __attribute__((visibility("default")))
#endif

typedef void (*ErrorCallback)(int, const char*);
typedef void (*BrowserCreatedCallback)(int);
typedef void (*BrowserClosingCallback)(int);
typedef void (*ContextCreatedCallback)(int, int);
typedef void (*QuerySuccessCallback)(long, const char*);
typedef void (*QueryFailureCallback)(long, int, const char*);
typedef bool (*QueryCallback)(int, int, long, const char*, QuerySuccessCallback, QueryFailureCallback);
