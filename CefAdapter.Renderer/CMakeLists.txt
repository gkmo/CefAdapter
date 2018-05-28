cmake_minimum_required(VERSION 3.0.0)
project(CefAdapter.Renderer VERSION 0.1.0)

include(CTest)
enable_testing()

include_directories("../Dependencies/cef/cef_binary_windows64")
link_directories("../Dependencies/cef/cef_binary_windows64/Debug" "../Dependencies/cef_wrapper")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

add_definitions(-DUNICODE)

add_executable(CefAdapter.Renderer src/Main.cpp)

target_link_libraries(CefAdapter.Renderer libcef libcef_dll_wrapper.lib)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
