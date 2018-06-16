#pragma once

#include "CefAdapterBrowserApplication.h"

extern "C"
{
	__declspec(dllexport) bool CreateApplication(HINSTANCE hInstance, const char* url, const char* subprocessPath, const char* logPath, 
		BrowserCreatedCallback browserCreatedCallback, ContextCreatedCallback contextCreatedCallback, ExecuteJsFunctionCallback executeJsFunctionCallback);

	__declspec(dllexport) void RunMessageLoop();

	__declspec(dllexport) void Shutdown();

	__declspec(dllexport) bool ExecuteJavaScript(int browserId, const char* code);

	__declspec(dllexport) bool ShowDeveloperTools(int browserId);

	__declspec(dllexport) void CreateJsGlobalFunction(int browserId, const char* name);

}