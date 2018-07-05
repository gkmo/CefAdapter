# Copyright (c) 2014 The Chromium Embedded Framework Authors. All rights
# reserved. Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.

#
# Source files.
#

# CefAdapter.Browser sources.
set(CEFADAPTER_BROWSER_SRCS
    src/CefAdapterBrowserApplication.cpp
    src/CefAdapterEventHandler.cpp
    src/CefAdapterMessageHandler.cpp
    src/CefAdapterInterProcessCommunicator.cpp
    src/CefAdapterRendererApplication
    src/Logger.cpp 
)

set(CEFADAPTER_BROWSER_SRCS_LINUX
  src/MainLinux.cpp
)

set(CEFADAPTER_BROWSER_SRCS_MACOSX
#   cefsimple_mac.mm
#   simple_handler_mac.mm
)

set(CEFADAPTER_BROWSER_SRCS_WINDOWS
    CefAdapter.Browser.exe.manifest 
    src/MainWindows.cpp
)

APPEND_PLATFORM_SOURCES(CEFADAPTER_BROWSER_SRCS)
source_group(CefAdapter.Browser FILES ${CEFADAPTER_BROWSER_SRCS})

# CefAdapter.Browser helper sources.
set(CEFADAPTER_BROWSER_HELPER_SRCS_MACOSX
  process_helper_mac.cc
)

APPEND_PLATFORM_SOURCES(CEFADAPTER_BROWSER_HELPER_SRCS)
source_group(CefAdapter.Browser FILES ${CEFADAPTER_BROWSER_HELPER_SRCS})

# CefAdapter.Browser resources.
set(CEFADAPTER_BROWSER_RESOURCES_MAC_SRCS_MACOSX
  mac/Info.plist
  mac/CefAdapter.Browser.icns
)

APPEND_PLATFORM_SOURCES(CEFADAPTER_BROWSER_RESOURCES_MAC_SRCS)
source_group(CefAdapter.Browser\\\\mac FILES ${CEFADAPTER_BROWSER_RESOURCES_MAC_SRCS})

set(CEFADAPTER_BROWSER_RESOURCES_MAC_ENGLISH_LPROJ_SRCS_MACOSX
  mac/English.lproj/InfoPlist.strings
  mac/English.lproj/MainMenu.xib
)

APPEND_PLATFORM_SOURCES(CEFADAPTER_BROWSER_RESOURCES_MAC_ENGLISH_LPROJ_SRCS)
source_group(CefAdapter.Browser\\\\mac\\\\English.lproj FILES ${CEFADAPTER_BROWSER_RESOURCES_MAC_ENGLISH_LPROJ_SRCS})

set(CEFADAPTER_BROWSER_RESOURCES_SRCS
  ${CEFADAPTER_BROWSER_RESOURCES_MAC_SRCS}
  ${CEFADAPTER_BROWSER_RESOURCES_MAC_ENGLISH_LPROJ_SRCS}
)

#
# Shared configuration.
#

# Target executable names.
set(CEF_TARGET "CefAdapter.Browser")
if(OS_MACOSX)
  set(CEF_HELPER_TARGET "CefAdapter.Browser_Helper")
  set(CEF_HELPER_OUTPUT_NAME "CefAdapter.Browser Helper")
endif()

# Logical target used to link the libcef library.
ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")

# Determine the target output directory.
SET_CEF_TARGET_OUT_DIR()


#
# Linux configuration.
#

if(OS_LINUX)
  include_directories("../Dependencies/cef_binary_linux64")  

  find_library(ZEROMQ_LIBRARY zmq)
  
  if(NOT ZEROMQ_LIBRARY)
    message(FATAL_ERROR "zmq library not found")
  endif()

  # Executable target.
  add_executable(${CEF_TARGET} ${CEFADAPTER_BROWSER_SRCS} ${CEFADAPTER_BROWSER_SRCS_LINUX})
  SET_EXECUTABLE_TARGET_PROPERTIES(${CEF_TARGET})
  add_dependencies(${CEF_TARGET} libcef_dll_wrapper)
  target_link_libraries(${CEF_TARGET} libcef_lib libcef_dll_wrapper ${ZEROMQ_LIBRARY} ${CEF_STANDARD_LIBS})    

  target_compile_options(${CEF_TARGET} PUBLIC -fexceptions)
  
  # Set rpath so that libraries can be placed next to the executable.
  set_target_properties(${CEF_TARGET} PROPERTIES INSTALL_RPATH "$ORIGIN")
  set_target_properties(${CEF_TARGET} PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
  set_target_properties(${CEF_TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CEF_TARGET_OUT_DIR})

  # Copy binary and resource files to the target output directory.
  COPY_FILES("${CEF_TARGET}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${CEF_TARGET_OUT_DIR}")
  COPY_FILES("${CEF_TARGET}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${CEF_TARGET_OUT_DIR}")

  # Set SUID permissions on the chrome-sandbox target.
  SET_LINUX_SUID_PERMISSIONS("${CEF_TARGET}" "${CEF_TARGET_OUT_DIR}/chrome-sandbox")  
endif()


#
# Mac OS X configuration.
#

if(OS_MACOSX)
  # Output paths for the app bundles.
  set(CEF_APP "${CEF_TARGET_OUT_DIR}/${CEF_TARGET}.app")
  set(CEF_HELPER_APP "${CEF_TARGET_OUT_DIR}/${CEF_HELPER_OUTPUT_NAME}.app")

  # Variable referenced from Info.plist files.
  set(PRODUCT_NAME "${CEF_TARGET}")

  # Helper executable target.
  add_executable(${CEF_HELPER_TARGET} MACOSX_BUNDLE ${CEFADAPTER_BROWSER_HELPER_SRCS})
  SET_EXECUTABLE_TARGET_PROPERTIES(${CEF_HELPER_TARGET})
  add_dependencies(${CEF_HELPER_TARGET} libcef_dll_wrapper)
  target_link_libraries(${CEF_HELPER_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})
  set_target_properties(${CEF_HELPER_TARGET} PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/mac/helper-Info.plist
    OUTPUT_NAME ${CEF_HELPER_OUTPUT_NAME}
    )

  # Fix the framework rpath in the helper executable.
  FIX_MACOSX_HELPER_FRAMEWORK_RPATH(${CEF_HELPER_TARGET})

  # Main executable target.
  add_executable(${CEF_TARGET} MACOSX_BUNDLE ${CEFADAPTER_BROWSER_RESOURCES_SRCS} ${CEFADAPTER_BROWSER_SRCS})
  SET_EXECUTABLE_TARGET_PROPERTIES(${CEF_TARGET})
  add_dependencies(${CEF_TARGET} libcef_dll_wrapper "${CEF_HELPER_TARGET}")
  target_link_libraries(${CEF_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})
  set_target_properties(${CEF_TARGET} PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/mac/Info.plist
    )

  # Copy files into the main app bundle.
  add_custom_command(
    TARGET ${CEF_TARGET}
    POST_BUILD
    # Copy the helper app bundle into the Frameworks directory.
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CEF_HELPER_APP}"
            "${CEF_APP}/Contents/Frameworks/${CEF_HELPER_OUTPUT_NAME}.app"
    # Copy the CEF framework into the Frameworks directory.
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CEF_BINARY_DIR}/Chromium Embedded Framework.framework"
            "${CEF_APP}/Contents/Frameworks/Chromium Embedded Framework.framework"
    VERBATIM
    )

  # Fix the framework rpath in the main executable.
  FIX_MACOSX_MAIN_FRAMEWORK_RPATH(${CEF_TARGET})

  # Manually process and copy over resource files.
  # The Xcode generator can support this via the set_target_properties RESOURCE
  # directive but that doesn't properly handle nested resource directories.
  # Remove these prefixes from input file paths.
  set(PREFIXES "mac/")
  COPY_MACOSX_RESOURCES("${CEFADAPTER_BROWSER_RESOURCES_SRCS}" "${PREFIXES}" "${CEF_TARGET}" "${CMAKE_CURRENT_SOURCE_DIR}" "${CEF_APP}")
endif()


#
# Windows configuration.
#

if(OS_WINDOWS)
  include_directories("../Dependencies/cef_binary_windows64")
  include_directories("../Dependencies/zmq/include")
  
  find_library(ZEROMQ_LIBRARY libzmq-v120-mt-4_0_4 HINTS "../Dependencies/zmq/lib")
  
  if(NOT ZEROMQ_LIBRARY)
    message(FATAL_ERROR "zmq library not found")
  endif()

  # Executable target.
  add_executable(${CEF_TARGET} WIN32 ${CEFADAPTER_BROWSER_SRCS})
  add_dependencies(${CEF_TARGET} libcef_dll_wrapper)  
  target_link_libraries(${CEF_TARGET} libcef_lib libcef_dll_wrapper ${ZEROMQ_LIBRARY} ${CEF_STANDARD_LIBS})

  SET_EXECUTABLE_TARGET_PROPERTIES(${CEF_TARGET})

  if(USE_SANDBOX)
    # Logical target used to link the cef_sandbox library.
    ADD_LOGICAL_TARGET("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
    target_link_libraries(${CEF_TARGET} cef_sandbox_lib ${CEF_SANDBOX_STANDARD_LIBS})
  endif()

  # Add the custom manifest files to the executable.
  ADD_WINDOWS_MANIFEST("${CMAKE_CURRENT_SOURCE_DIR}" "${CEF_TARGET}" "exe")

  COPY_FILES("${CEF_TARGET}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${CEF_TARGET_OUT_DIR}")
  COPY_FILES("${CEF_TARGET}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${CEF_TARGET_OUT_DIR}")  

endif()