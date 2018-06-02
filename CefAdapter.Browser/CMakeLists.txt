cmake_minimum_required(VERSION 3.0.0)
project(CefAdapter.Browser VERSION 0.1.0)

include(CTest)
enable_testing()

include_directories("../Dependencies/cef/cef_binary_windows64")
link_directories("../Dependencies/cef/cef_binary_windows64/Debug" "../Dependencies/cef_wrapper")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} /MT /RTC1 /Od /MP /Gy /GR- /W4 /WX /wd4100 /wd4127 /wd4244 /wd4481 /wd4512 /wd4701 /wd4702 /wd4996 /Zi")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd /RTC1 /Od /MP /Gy /GR- /W4 /WX /wd4100 /wd4127 /wd4244 /wd4481 /wd4512 /wd4701 /wd4702 /wd4996 /Zi")

add_definitions(-DUNICODE)

add_library(CefAdapter.Browser SHARED src/CefAdapterExport.cpp src/CefAdapterBrowserApplication.cpp src/CefAdapterEventHandler.cpp)

target_link_libraries(CefAdapter.Browser libcef.lib libcef_dll_wrapper.lib comctl32.lib rpcrt4.lib shlwapi.lib ws2_32.lib)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
