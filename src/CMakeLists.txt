# Choose your version!
set(SKYRIM_AE ON)
set(SKYRIM_SE OFF)
set(SKYRIM_VR OFF)

# Would you like to output the generated SKSE plugin .dll to a directory?
set(OUTPUT_DLL ON)

###################################################

set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(SOURCE_DIR "${ROOT_DIR}/src")

# Set Project Source Files
set(SOURCE_FILES
	"${SOURCE_DIR}/main.cpp"
	"${SOURCE_DIR}/PCH.h"

	"${SOURCE_DIR}/Events.h"

	"${SOURCE_DIR}/Papyrus.h"
	"${SOURCE_DIR}/Papyrus/Ini.h"
	"${SOURCE_DIR}/Papyrus/DateTime.h"
	"${SOURCE_DIR}/Papyrus/FileSystem.h"
	"${SOURCE_DIR}/Papyrus/Reflection.h"

	"${SOURCE_DIR}/Web.h"
	"${SOURCE_DIR}/Web/AppComponent.hpp"
	"${SOURCE_DIR}/Web/controllers/HelloController.cpp"
	"${SOURCE_DIR}/Web/controllers/HelloController.hpp"
	"${SOURCE_DIR}/Web/dtos/TestDto.hpp"
)

source_group(TREE "${ROOT_DIR}" FILES ${SOURCE_FILES})

# Setup Plugin.h configuration template
set(VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/Plugin.h")
configure_file(
	"${ROOT_DIR}/cmake/Plugin.h.in"
	"${VERSION_HEADER}"
	@ONLY
)

# Setup version.rb configuration template
source_group("src" FILES "${VERSION_HEADER}")
configure_file(
	"${ROOT_DIR}/cmake/version.rc.in"
	"${CMAKE_CURRENT_BINARY_DIR}/version.rc"
	@ONLY
)

# Setup SKSE plugin as a library
add_library(
	"${PROJECT_NAME}"
	SHARED
	${SOURCE_FILES}
	"${VERSION_HEADER}"
	"${CMAKE_CURRENT_BINARY_DIR}/version.rc"
	"${ROOT_DIR}/.clang-format"
	"${ROOT_DIR}/.editorconfig"
)

# Compile with C++20 features
target_compile_features(
	"${PROJECT_NAME}"
	PRIVATE
		cxx_std_23
)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	target_compile_options(
		"${PROJECT_NAME}"
		PRIVATE
			"/sdl"	# Enable Additional Security Checks
			"/utf-8"	# Set Source and Executable character sets to UTF-8
			"/Zi"	# Debug Information Format

			"/permissive-"	# Standards conformance
			"/Zc:preprocessor"	# Enable preprocessor conformance mode

			"/wd4200" # nonstandard extension used : zero-sized array in struct/union

			"$<$<CONFIG:DEBUG>:>"
			"$<$<CONFIG:RELEASE>:/Zc:inline;/JMC-;/Ob3>"
	)

	target_link_options(
		"${PROJECT_NAME}"
		PRIVATE
			"$<$<CONFIG:DEBUG>:/INCREMENTAL;/OPT:NOREF;/OPT:NOICF>"
			"$<$<CONFIG:RELEASE>:/INCREMENTAL:NO;/OPT:REF;/OPT:ICF;/DEBUG:FULL>"
	)
endif()

target_include_directories(
	"${PROJECT_NAME}"
	PRIVATE
		"${CMAKE_CURRENT_BINARY_DIR}/src"
		"${SOURCE_DIR}"
)

# Web framework: Oatpp
find_package(oatpp 1.3.0 REQUIRED)
target_link_libraries("${PROJECT_NAME}" PRIVATE PUBLIC oatpp::oatpp)

###################################################
# TODO Set this up for AE, SE, and VR
###################################################
# Load CommonLibSSE from vcpkg
find_package(CommonLibSSE CONFIG REQUIRED)
target_link_libraries("${PROJECT_NAME}" PRIVATE CommonLibSSE::CommonLibSSE)

target_precompile_headers(
	"${PROJECT_NAME}"
	PRIVATE
		"${SOURCE_DIR}/PCH.h"
)

install(
	FILES
		"$<TARGET_FILE:${PROJECT_NAME}>"
	DESTINATION "SKSE/Plugins"
	COMPONENT "main"
)

install(
	FILES
		"$<TARGET_PDB_FILE:${PROJECT_NAME}>"
	DESTINATION "/"
	COMPONENT "pdbs"
)

if("${COPY_OUTPUT}")
	add_custom_command(
		TARGET "${PROJECT_NAME}"
		POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:${PROJECT_NAME}>" "${Skyrim64Path}/Data/SKSE/Plugins/"
		COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_PDB_FILE:${PROJECT_NAME}>" "${Skyrim64Path}/Data/SKSE/Plugins/"
		VERBATIM
	)
endif()

if("${OUTPUT_DLL}")
	if ("${SKYRIM_AE}")
		if (EXISTS "$ENV{SKYRIM_AE_SKSE_DIR}")
			message(STATUS "Output SKSE DLL for AE to $ENV{SKYRIM_AE_SKSE_DIR}")
			add_custom_command(
				TARGET "${PROJECT_NAME}"
				POST_BUILD
				COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:${PROJECT_NAME}>" "$ENV{SKYRIM_AE_SKSE_DIR}/"
				COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_PDB_FILE:${PROJECT_NAME}>" "$ENV{SKYRIM_AE_SKSE_DIR}/"
				VERBATIM
			)
		else()
			message(WARNING "Could not output SKSE dll for AE. Output directory not found: $ENV{SKYRIM_AE_SKSE_DIR}")
		endif()
	elseif("${SKYRIM_SE}")
		if (EXISTS "$ENV{SKYRIM_SE_SKSE_DIR}")
			message(STATUS "Output SKSE DLL for SE to $ENV{SKYRIM_SE_SKSE_DIR}")
			add_custom_command(
				TARGET "${PROJECT_NAME}"
				POST_BUILD
				COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:${PROJECT_NAME}>" "$ENV{SKYRIM_SE_SKSE_DIR}/"
				COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_PDB_FILE:${PROJECT_NAME}>" "$ENV{SKYRIM_SE_SKSE_DIR}/"
				VERBATIM
			)
		else()
			message(WARNING "Could not output SKSE dll for SE. Output directory not found: $ENV{SKYRIM_SE_SKSE_DIR}")
		endif()
	elseif("${SKYRIM_VR}")
		if (EXISTS "$ENV{SKYRIM_VR_SKSE_DIR}")
			message(STATUS "Output SKSE DLL for VR to $ENV{SKYRIM_VR_SKSE_DIR}")
			add_custom_command(
				TARGET "${PROJECT_NAME}"
				POST_BUILD
				COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:${PROJECT_NAME}>" "$ENV{SKYRIM_VR_SKSE_DIR}/"
				COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_PDB_FILE:${PROJECT_NAME}>" "$ENV{SKYRIM_VR_SKSE_DIR}/"
				VERBATIM
			)
		else()
			message(WARNING "Could not output SKSE dll for VR. Output directory not found: $ENV{SKYRIM_VR_SKSE_DIR}")
		endif()
	endif()
endif()

# Unset configuration variables (or they may be cached)
unset(SKYRIM_AE)
unset(SKYRIM_SE)
unset(SKYRIM_VR)
unset(OUTPUT_DLL)
