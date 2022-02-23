# Welcome to this lovely CMake file <3
cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Your SKSE plugin name and version
# These become available via Plugin.h
project(
	MyFirstSkse
	VERSION 0.420.69
	LANGUAGES CXX
)

# Process the .in templates in the cmake/ folder for Plugin.h and version.rc
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# Load the CMakeLists.txt in the src folder
add_subdirectory(src)
