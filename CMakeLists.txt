# Welcome to this lovely CMake file <3
cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(VCPKG_OVERLAY_TRIPLETS "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(VCPKG_TARGET_TRIPLET "x64-windows-skse")

# Your SKSE plugin name and version
# These become available via Plugin.h
project(
	VRKeyboard
	VERSION 0.420.69
	LANGUAGES CXX
)

# Process the .in templates in the cmake/ folder for Plugin.h and version.rc
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# ?
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}")

# Load the CMakeLists.txt in the src folder
add_subdirectory(src)
