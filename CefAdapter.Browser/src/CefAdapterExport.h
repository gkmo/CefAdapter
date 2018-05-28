#pragma once

#include "stdafx.h"
#include "CefAdapterApplication.h"

extern "C"
{
	__declspec(dllexport) bool CreateApplication(HINSTANCE hInstance, const char* url, const char* subprocessPath, BrowserCreatedCallback browserCreatedCallback);

	__declspec(dllexport) void RunMessageLoop();

	__declspec(dllexport) void Shutdown();

	__declspec(dllexport) bool ExecuteJavaScript(int browserId, const char* code);

	__declspec(dllexport) bool ShowDeveloperTools(int browserId);
}