#
# Copyright (C) 2016 necropotame (necropotame@gmail.com)
#
# This file is part of TeeUniverses.
#
# TeeUniverses is free software: you can redistribute it and/or  modify
# it under the terms of the GNU Affero General Public License, version 3,
# as published by the Free Software Foundation.
#
# TeeUniverses is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with TeeUniverses.  If not, see <http://www.gnu.org/licenses/>.
#

cmake_minimum_required (VERSION 2.6)
project(TeeUniverses)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/modules)

########################################################################
# DEPENDENCIES
########################################################################

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(PNG REQUIRED)
find_package(Freetype REQUIRED)
find_package(HarfBuzz REQUIRED)
find_package(ICU COMPONENTS i18n  REQUIRED)

########################################################################
# OPTIONS
########################################################################

set(TU_EXECNAME_GAME teeuniverses)
set(TU_EXECNAME_EDITOR teeuniverses_editor)
set(TU_EXECNAME_SERVER teeuniverses_srv)

include_directories(
	"${PROJECT_SOURCE_DIR}/src"
	${SDL2_INCLUDE_DIR}
	${OPENGL_INCLUDE_DIRS}
	${PNG_INCLUDE_DIR}
	${FREETYPE_INCLUDE_DIRS}
	${HARFBUZZ_INCLUDE_DIRS}
	${ICU_INCLUDE_DIRS}
)

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

########################################################################
# SHARED
########################################################################

# Sources
file(GLOB teeuniverses_shared_src
    "${PROJECT_SOURCE_DIR}/src/external/zlib/*.c"
    "${PROJECT_SOURCE_DIR}/src/external/json-parser/*.c"
    "${PROJECT_SOURCE_DIR}/src/shared/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/shared/system/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/shared/tl/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/shared/assets/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/shared/components/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/tw07/shared/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/generated/assets/*.cpp"
)

# Build
add_library(teeuniverses_shared ${teeuniverses_shared_src})
target_link_libraries (teeuniverses_shared
	${ICU_LIBRARIES}
)

########################################################################
# CLIENT
########################################################################

# Sources
file(GLOB teeuniverses_client_src
    "${PROJECT_SOURCE_DIR}/src/external/pnglite/*.c"
    "${PROJECT_SOURCE_DIR}/src/client/gui/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/client/components/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/client/*.cpp"
)

# Libraries
set(teeuniverses_client_libraries
	teeuniverses_shared
	${SDL2_LIBRARY}
	${PNG_LIBRARY}
	${OPENGL_LIBRARIES}
	${FREETYPE_LIBRARIES}
	${HARFBUZZ_LIBRARIES}
)

# Build
add_library(teeuniverses_client ${teeuniverses_client_src})
target_link_libraries (teeuniverses_client ${teeuniverses_client_libraries})

########################################################################
# GAME
########################################################################

# Sources
file(GLOB teeuniverses_game_src
    "${PROJECT_SOURCE_DIR}/src/game/gui/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/game/components/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/game/*.cpp"
)

# Libraries
set(teeuniverses_game_libraries
	teeuniverses_client
)

# Build Editor
add_executable(${TU_EXECNAME_GAME} "${PROJECT_SOURCE_DIR}/src/game.cpp" ${teeuniverses_game_src})
target_link_libraries (${TU_EXECNAME_GAME} ${teeuniverses_game_libraries})

########################################################################
# EDITOR
########################################################################

# Sources
file(GLOB teeuniverses_editor_src
    "${PROJECT_SOURCE_DIR}/src/editor/gui/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/editor/components/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/editor/*.cpp"
)

# Libraries
set(teeuniverses_editor_libraries
	teeuniverses_client
)

# Build Editor
add_executable(${TU_EXECNAME_EDITOR} "${PROJECT_SOURCE_DIR}/src/editor.cpp" ${teeuniverses_editor_src})
target_link_libraries (${TU_EXECNAME_EDITOR} ${teeuniverses_editor_libraries})

########################################################################
# SERVER
########################################################################

# Sources
file(GLOB teeuniverses_server_src
    "${PROJECT_SOURCE_DIR}/src/server/*.cpp"
)

# Libraries
set(teeuniverses_server_libraries
	teeuniverses_shared
)

# Build
add_executable(${TU_EXECNAME_SERVER} "${PROJECT_SOURCE_DIR}/src/server.cpp" ${teeuniverses_server_src})
set_target_properties(${TU_EXECNAME_SERVER} PROPERTIES COMPILE_FLAGS "-DKERNEL_TYPE=KERNEL_SERVER")
target_link_libraries (${TU_EXECNAME_SERVER} ${teeuniverses_server_libraries})

########################################################################
# TOOLS
########################################################################

# Sources
file(GLOB teeuniverses_tools_src
    "${PROJECT_SOURCE_DIR}/src/external/pnglite/*.c"
    "${PROJECT_SOURCE_DIR}/src/client/loading_tools.cpp"
)

# Libraries
set(teeuniverses_tools_libraries
	teeuniverses_shared
	${PNG_LIBRARY}
)

# Build
add_library(teeuniverses_tools ${teeuniverses_tools_src})
target_link_libraries (teeuniverses_tools ${teeuniverses_tools_libraries})

# Make Assets
function(make_assets progname)
	add_executable(make_assets_${progname} ${PROJECT_SOURCE_DIR}/src/make_assets_${progname}.cpp)
	target_link_libraries(
		make_assets_${progname}
		teeuniverses_tools
	)
endfunction()

make_assets(gui_editor)
make_assets(univ_teeworlds)
make_assets(univ_ddnet)
make_assets(univ_infclass)
make_assets(univ_openfng)

########################################################################
# UNIT TEST
########################################################################

enable_testing()

function(make_test testname)
	add_executable(test_${testname} src/unittest/${testname}.cpp)
	target_link_libraries(
		test_${testname}
		teeuniverses_shared
	)
	add_test(NAME test_${testname} COMMAND $<TARGET_FILE:test_${testname}>)
endfunction()

make_test(system_string)
make_test(system_fs)
make_test(shared_kernel)
make_test(localization)
make_test(archivefile)
