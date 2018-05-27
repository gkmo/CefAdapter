cmake_minimum_required(VERSION 3.0.0)
project(CefAdapter.Native VERSION 0.1.0)

include(CTest)
enable_testing()

include_directories("../third_party/cef/cef_binary_3.3325.1756.g6d8faa4_windows64")
link_directories("../third_party/cef/cef_binary_3.3325.1756.g6d8faa4_windows64/Debug" "../third_party/cef_wrapper")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

add_library(CefAdapter.Native SHARED CefAdapterExport.cpp CefAdapterApplication.cpp CefAdapterEventHandler.cpp)

target_link_libraries(CefAdapter.Native libcef libcef_dll_wrapper.lib)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
