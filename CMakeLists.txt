cmake_minimum_required(VERSION 3.0.0)
project(CefAdapter VERSION 0.1.0)

if(NOT CMAKE_BUILD_TYPE) 
    set(CMAKE_BUILD_TYPE Debug)
endif(NOT CMAKE_BUILD_TYPE)

# Add custom manifest files to an executable target.
macro(ADD_WINDOWS_MANIFEST manifest_path target extension)
  add_custom_command(
    TARGET ${target}
    POST_BUILD
    COMMAND "mt.exe" -nologo -manifest \"${manifest_path}/${target}.${extension}.manifest\" -outputresource:"${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${target}.${extension}"\;\#1
    COMMENT "Adding manifest..."
    )
endmacro()

add_subdirectory(CefAdapter.Browser)
add_subdirectory(CefAdapter.Renderer)