cmake_minimum_required(VERSION 3.0.0)
project(CefAdapter.Renderer VERSION 0.1.0)

include(CTest)
enable_testing()

include_directories("../Dependencies/cef/cef_binary_windows64")
link_directories("../Dependencies/cef/cef_binary_windows64/Debug" "../Dependencies/cef_wrapper")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} /MT /RTC1 /Od /MP /Gy /GR- /W4 /WX /wd4100 /wd4127 /wd4244 /wd4481 /wd4512 /wd4701 /wd4702 /wd4996 /Zi")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd /RTC1 /Od /MP /Gy /GR- /W4 /WX /wd4100 /wd4127 /wd4244 /wd4481 /wd4512 /wd4701 /wd4702 /wd4996 /Zi")

add_definitions(-DUNICODE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -DWIN32 -D_WIN32 -D_WINDOWS -D_UNICODE -DWINVER=0x0601 -DNOMINMAX -DWIN32_LEAN_AND_MEAN -D_HAS_EXCEPTIONS=0 -DCEF_USE_ATL)

add_executable(CefAdapter.Renderer WIN32 src/CefAdapter.Renderer.exe.manifest src/Main.cpp src/CefAdapterRendererApplication.cpp)

target_link_libraries(CefAdapter.Renderer libcef.lib libcef_dll_wrapper.lib comctl32.lib rpcrt4.lib shlwapi.lib ws2_32.lib)

# Add the custom manifest files to the executable.
ADD_WINDOWS_MANIFEST("${CMAKE_CURRENT_SOURCE_DIR}/src" "CefAdapter.Renderer" "exe")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
