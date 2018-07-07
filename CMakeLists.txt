#
# Global setup.
#

cmake_minimum_required(VERSION 2.8.12.1)

# Only generate Debug and Release configuration types.
set(CMAKE_CONFIGURATION_TYPES Debug Release)

# Project name.
project(CefAdapter)

set_property(GLOBAL PROPERTY OS_FOLDERS ON)

if (${CMAKE_SIZEOF_VOID_P} EQUAL "8")
    set (PROJECTNAME_ARCHITECTURE "x64")
else ()
    set (PROJECTNAME_ARCHITECTURE "x86")
endif ()

if (UNIX)
    if (${PROJECTNAME_ARCHITECTURE} EQUAL "x64")
        set(CEF_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/cef_binary_linux64")
    else ()    
        set(CEF_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/cef_binary_linux32")
    endif ()
endif (UNIX)

if (WIN32)
    if (${PROJECTNAME_ARCHITECTURE} EQUAL "x64")
        set(CEF_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/cef_binary_windows64")
        set(ZMQ_DIR "C:/Program Files/ZeroMQ 4.0.4")
    else ()    
        set(CEF_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/cef_binary_windows32")
        set(ZMQ_DIR "C:/Program Files (x86)/ZeroMQ 4.0.4")
    endif ()
endif (WIN32)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")

# Execute FindCEF.cmake which must exist in CMAKE_MODULE_PATH.
find_package(CEF REQUIRED)

# Include the libcef_dll_wrapper target.
# Comes from the libcef_dll/CMakeLists.txt file in the binary distribution
# directory.
#add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)

# Include application targets.
# Comes from the <target>/CMakeLists.txt file in the current directory.
# TODO: Change these lines to match your project target when you copy this file.
add_subdirectory(CefAdapter)

# Display configuration settings.
PRINT_CEF_CONFIG()