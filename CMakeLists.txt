#
# Copyright (C) 2016 necropotame (necropotame@gmail.com)
#
# This file is part of TeeUniverse.
#
# TeeUniverse is free software: you can redistribute it and/or  modify
# it under the terms of the GNU Affero General Public License, version 3,
# as published by the Free Software Foundation.
#
# TeeUniverse is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with TeeUniverse.  If not, see <http://www.gnu.org/licenses/>.
#

cmake_minimum_required (VERSION 2.6)
cmake_policy(SET CMP0012 NEW)

project(TeeUniverse)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/modules)

########################################################################
# DEPENDENCIES
########################################################################

find_package(SDL2)
find_package(OpenGL)
find_package(Freetype)
if(NOT(WITHOUT_HARFBUZZ))
	find_package(HarfBuzz)
endif()
find_package(ICU COMPONENTS i18n)

########################################################################
# BUILD_TYPE, PLATFORM
########################################################################

if(NOT(CMAKE_BUILD_TYPE))
	set(CMAKE_BUILD_TYPE Release)
endif()

string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)

if(NOT(PLATFORM))
	if(WIN32)
		set(PLATFORM win)
	else()
		add_definitions(-DCONF_FAMILY_UNIX)
		if(APPLE)
			set(PLATFORM osx)
		else()
			set(PLATFORM linux)
		endif()
	endif()
endif()

########################################################################
# OPTIONS
########################################################################

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/${BUILD_TYPE}/${PLATFORM}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/${BUILD_TYPE}/${PLATFORM}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/${BUILD_TYPE}/${PLATFORM})
set(DATA_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/${BUILD_TYPE}/${PLATFORM}/data)

set(TU_EXECNAME_GAME teeuniverse)
set(TU_EXECNAME_EDITOR teeuniverse_editor)
set(TU_EXECNAME_SERVER teeuniverse_srv)

include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/src/external")
include_directories("${PROJECT_SOURCE_DIR}/src/external/zlib")

if(SDL2_FOUND)
	include_directories(${SDL2_INCLUDE_DIR})
endif()
if(OPENGL_FOUND)
	include_directories(${OPENGL_INCLUDE_DIRS})
endif()
if(FREETYPE_FOUND)
	include_directories(${FREETYPE_INCLUDE_DIRS})
endif()
if(HARFBUZZ_FOUND)
	add_definitions(-DHARFBUZZ_ENABLED)
	include_directories(${HARFBUZZ_INCLUDE_DIRS})
endif()
if(ICU_FOUND)
	add_definitions(-DU_USING_ICU_NAMESPACE)
	include_directories(${ICU_INCLUDE_DIRS})
endif()

message(STATUS "******** TeeUniverse ********")
message(STATUS "Build type: ${BUILD_TYPE}")
message(STATUS "Platform: ${PLATFORM}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "Install directory: ${CMAKE_INSTALL_PREFIX}")

message(STATUS ${name} "Dependencies:")
function(show_dependency_status name varFound error)
	if(${varFound})
		message(STATUS " * " ${name} " found")
	else()
		message(STATUS " * " ${name} " not found (" ${error} ")")
	endif()
endfunction()

show_dependency_status("ICU" "${ICU_FOUND}" "required by server, client, editor, tools")
show_dependency_status("SDL" "${SDL2_FOUND}" "required by: client, editor")
show_dependency_status("OpenGL" "${OPENGL_FOUND}" "required by: client, editor")
show_dependency_status("FreeType" "${FREETYPE_FOUND}" "required by: client, editor")
show_dependency_status("Harfbuzz" "${HARFBUZZ_FOUND}" "used by: client, editor")

if(NOT(ICU_FOUND))
	message(FATAL_ERROR "You must install ICU to compile TeeUniverse")
endif()

if(NOT(HARFBUZZ_FOUND) AND NOT(WITHOUT_HARFBUZZ))
	message(STATUS " ! Warning: Without Harfbuzz, the text rendering")
	message(STATUS "            quality is reduced. In particular,")
	message(STATUS "            arabic and farsi can't be displayed")
	message(STATUS "            correctly.")
endif()

message(STATUS ${name} "Target status:")

########################################################################
# DEFINES
########################################################################

if(WIN32)
	add_definitions(-DCONF_FAMILY_WINDOWS)
else()
	add_definitions(-DCONF_FAMILY_UNIX)
	if(APPLE)
		add_definitions(-DCONF_PLATFORM_MACOSX)
	endif()
endif()

if(CMAKE_COMPILER_IS_GNUCC)
    add_definitions("-Wall -Wextra -Wno-unused-parameter -Wno-maybe-uninitialized")
endif()

########################################################################
# SHARED
########################################################################

# Sources
file(GLOB teeuniverse_shared_src
    "${PROJECT_SOURCE_DIR}/src/external/zlib/*.c"
    "${PROJECT_SOURCE_DIR}/src/external/json-parser/*.c"
    "${PROJECT_SOURCE_DIR}/src/shared/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/shared/system/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/shared/tl/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/shared/assets/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/shared/components/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/shared/math/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/shared/geometry/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/tw07/shared/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/generated/assets/*.cpp"
)

# Build
add_library(teeuniverse_shared ${teeuniverse_shared_src})
target_link_libraries (teeuniverse_shared
	${ICU_LIBRARIES}
)
add_custom_command(TARGET teeuniverse_shared POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data ${DATA_OUTPUT_DIRECTORY})
install(DIRECTORY "data/languages" DESTINATION "share/teeuniverse")
install(DIRECTORY "data/assets" DESTINATION "share/teeuniverse")

########################################################################
# CLIENT
########################################################################

if(SDL2_FOUND AND OPENGL_FOUND AND FREETYPE_FOUND)

	# Sources
	file(GLOB teeuniverse_client_src
		"${PROJECT_SOURCE_DIR}/src/external/pnglite/*.c"
		"${PROJECT_SOURCE_DIR}/src/client/gui/*.cpp"
		"${PROJECT_SOURCE_DIR}/src/client/components/*.cpp"
		"${PROJECT_SOURCE_DIR}/src/client/*.cpp"
	)

	# Libraries
	set(teeuniverse_client_libraries
		teeuniverse_shared
		${SDL2_LIBRARY}
		${OPENGL_LIBRARIES}
		${FREETYPE_LIBRARIES}
	)
	
	if(HARFBUZZ_FOUND)
		set(teeuniverse_client_libraries ${teeuniverse_client_libraries} ${HARFBUZZ_LIBRARIES})
	endif()

	# Build
	add_library(teeuniverse_client ${teeuniverse_client_src})
	target_link_libraries (teeuniverse_client ${teeuniverse_client_libraries})
	install(DIRECTORY "data/countryflags" DESTINATION "share/teeuniverse")
	install(DIRECTORY "data/fonts" DESTINATION "share/teeuniverse")

	########################################################################
	# GAME
	########################################################################

	# Sources
	#file(GLOB teeuniverse_game_src
	#	"${PROJECT_SOURCE_DIR}/src/game/gui/*.cpp"
	#	"${PROJECT_SOURCE_DIR}/src/game/components/*.cpp"
	#	"${PROJECT_SOURCE_DIR}/src/game/*.cpp"
	#)

	# Libraries
	#set(teeuniverse_game_libraries
	#	teeuniverse_client
	#)

	# Build Game
	#add_executable(${TU_EXECNAME_GAME} "${PROJECT_SOURCE_DIR}/src/game.cpp" ${teeuniverse_game_src})
	#target_link_libraries (${TU_EXECNAME_GAME} ${teeuniverse_game_libraries})
	#install(TARGETS ${TU_EXECNAME_GAME} DESTINATION bin)
	#message(STATUS " * client ready for compilation")
	message(STATUS " * client not yet implemented")

	########################################################################
	# EDITOR
	########################################################################

	# Sources
	file(GLOB teeuniverse_editor_src
		"${PROJECT_SOURCE_DIR}/src/editor/gui/*.cpp"
		"${PROJECT_SOURCE_DIR}/src/editor/components/*.cpp"
		"${PROJECT_SOURCE_DIR}/src/editor/*.cpp"
	)

	# Libraries
	set(teeuniverse_editor_libraries
		teeuniverse_client
	)

	# Build Editor
	add_executable(${TU_EXECNAME_EDITOR} "${PROJECT_SOURCE_DIR}/src/editor.cpp" ${teeuniverse_editor_src})
	target_link_libraries (${TU_EXECNAME_EDITOR} ${teeuniverse_editor_libraries})
	install(TARGETS ${TU_EXECNAME_EDITOR} DESTINATION bin)
	message(STATUS " * editor ready for compilation")
	
else()
	#message(STATUS " * client can't be compiled")
	message(STATUS " * editor can't be compiled")
endif()

########################################################################
# SERVER
########################################################################

# Sources
#file(GLOB teeuniverse_server_src
#    "${PROJECT_SOURCE_DIR}/src/server/*.cpp"
#)

# Libraries
#set(teeuniverse_server_libraries
#	teeuniverse_shared
#)

# Build
#add_executable(${TU_EXECNAME_SERVER} "${PROJECT_SOURCE_DIR}/src/server.cpp" ${teeuniverse_server_src})
#set_target_properties(${TU_EXECNAME_SERVER} PROPERTIES COMPILE_FLAGS "-DKERNEL_TYPE=KERNEL_SERVER")
#target_link_libraries (${TU_EXECNAME_SERVER} ${teeuniverse_server_libraries})
#install(TARGETS ${TU_EXECNAME_SERVER} DESTINATION bin)
#message(STATUS " * server ready for compilation")
message(STATUS " * server not yet implemented")

########################################################################
# TOOLS
########################################################################

# Sources
file(GLOB teeuniverse_tools_src
	"${PROJECT_SOURCE_DIR}/src/external/pnglite/*.c"
	"${PROJECT_SOURCE_DIR}/src/client/loading_tools.cpp"
)

# Libraries
set(teeuniverse_tools_libraries
	teeuniverse_shared
)

# Build
add_library(teeuniverse_tools ${teeuniverse_tools_src})
target_link_libraries (teeuniverse_tools ${teeuniverse_tools_libraries})

# Make Assets
function(make_assets progname)
	add_executable(make_assets_${progname} ${PROJECT_SOURCE_DIR}/src/make_assets_${progname}.cpp)
	set_target_properties(
		make_assets_${progname}
		PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/make_assets"
		LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/make_assets"
		RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/make_assets"
	)
	target_link_libraries(
		make_assets_${progname}
		teeuniverse_tools
	)
	add_custom_command(TARGET make_assets_${progname} POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory "${DATA_OUTPUT_DIRECTORY}/assets")
	add_custom_command(TARGET make_assets_${progname} POST_BUILD COMMAND make_assets_${progname} --data-dir "${CMAKE_SOURCE_DIR}/datasrc" --data-dir "${CMAKE_SOURCE_DIR}/data" --save-dir "${DATA_OUTPUT_DIRECTORY}" --save-dir-noinit)

endfunction()

make_assets(gui_editor)
make_assets(teeworlds)
make_assets(ddnet)
make_assets(infclass)
make_assets(openfng)

message(STATUS " * tools ready for compilation")

########################################################################
# UNIT TEST
########################################################################

enable_testing()

function(make_test testname)
	add_executable(test_${testname} src/unittest/${testname}.cpp)
	set_target_properties(
		test_${testname}
		PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/test"
		LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/test"
		RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test"
	)
	target_link_libraries(
		test_${testname}
		teeuniverse_shared
	)
	add_test(NAME test_${testname} COMMAND $<TARGET_FILE:test_${testname}>)
endfunction()

make_test(system_string)
make_test(system_fs)
make_test(shared_kernel)
make_test(localization)
make_test(archivefile)
make_test(assetsmanager)

message(STATUS "******************************")
