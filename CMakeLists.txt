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

# Freetype::Freetype is available since CMake 3.10
cmake_minimum_required(VERSION 3.10)

project(TeeUniverse
    VERSION 0.3.2
)

option(USE_CONAN "Use Conan for dependencies" OFF)
option(USE_HARFBUZZ "Enable HarfBuzz for better text shaping" ON)

if(USE_CONAN)
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}")
  list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/modules")

# Backward compatibility:
if(WITHOUT_HARFBUZZ)
  set(USE_HARFBUZZ OFF)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

########################################################################
# BUILD_TYPE, PLATFORM
########################################################################

if(CMAKE_CONFIGURATION_TYPES) # multiconfig generator
  set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
else()
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
  endif()
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release")
  string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
endif()

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

if(BUILD_TYPE)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/${BUILD_TYPE}/${PLATFORM}/lib)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/${BUILD_TYPE}/${PLATFORM}/lib)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/${BUILD_TYPE}/${PLATFORM})
else()
  foreach(CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES})
    string(TOLOWER ${CONFIG_TYPE} CONFIG_TYPE_DIR)
    string(TOUPPER ${CONFIG_TYPE} CONFIG_TYPE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/build/${CONFIG_TYPE_DIR}/${PLATFORM}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/build/${CONFIG_TYPE_DIR}/${PLATFORM}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/build/${CONFIG_TYPE_DIR}/${PLATFORM})
  endforeach(CONFIG_TYPE)
endif()

set(DATA_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/$<LOWER_CASE:$<CONFIG>>/${PLATFORM}/data)

set(TU_EXECNAME_GAME teeuniverse)
set(TU_EXECNAME_EDITOR teeuniverse_editor)
set(TU_EXECNAME_SERVER teeuniverse_srv)

########################################################################
# DEPENDENCIES
########################################################################

if(USE_CONAN)
  set(CMAKE_CXX_STANDARD 14)

  include(modules/conan.cmake)

  conan_cmake_configure(
    REQUIRES
      sdl/2.0.16
      freetype/2.11.1
      icu/69.1
      zlib/1.2.11
    GENERATORS cmake_find_package
  )

  conan_cmake_autodetect(settings)

  conan_cmake_install(PATH_OR_REFERENCE .
                      BUILD missing
                      REMOTE conancenter
                      SETTINGS ${settings})
endif()

find_package(SDL2)
find_package(OpenGL)
find_package(Freetype)
if(USE_HARFBUZZ)
  find_package(harfbuzz REQUIRED)
endif()
find_package(ICU REQUIRED COMPONENTS uc i18n)
find_package(ZLIB)

include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/src/external")
include_directories("${PROJECT_SOURCE_DIR}/src/external/minizip")

if(ICU_FOUND)
  add_definitions(-DU_USING_ICU_NAMESPACE)
endif()

message(STATUS "******** TeeUniverse ********")
if(BUILD_TYPE)
message(STATUS "Build type: ${BUILD_TYPE}")
else()
message(STATUS "Build configuration: ${CMAKE_CONFIGURATION_TYPES}")
endif()
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
if(USE_HARFBUZZ)
  show_dependency_status("Harfbuzz" "${harfbuzz_FOUND}" "used by: client, editor")
else()
  show_dependency_status("Harfbuzz" "(disabled)" "used by: client, editor")
endif()
show_dependency_status("ZLib" "${ZLIB_FOUND}" "required by: server, client, editor, tools")

if(USE_HARFBUZZ AND NOT(harfbuzz_FOUND))
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

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions("-Wall -Wextra -Wno-unused-parameter -Wno-maybe-uninitialized")
endif()

if(MINGW)
  SET(CMAKE_C_LINK_EXECUTABLE "${CMAKE_C_LINK_EXECUTABLE} --static -static-libgcc")
  SET(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} --static -static-libgcc -static-libstdc++")
endif()

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /EHsc /GS")
  # Disable warnings:
  # C4065: switch statement contains 'default' but no 'case' labels
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4065")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4244 /wd4267 /wd4800 /wd4996")
endif()

########################################################################
# SHARED
########################################################################

configure_file("src/version.h.in" "src/shared/version.h")

# Sources
file(GLOB teeuniverse_shared_src
  "${PROJECT_SOURCE_DIR}/src/external/minizip/ioapi.c"
  "${PROJECT_SOURCE_DIR}/src/external/minizip/miniunz.c"
  "${PROJECT_SOURCE_DIR}/src/external/minizip/minizip.c"
  "${PROJECT_SOURCE_DIR}/src/external/minizip/zip.c"
  "${PROJECT_SOURCE_DIR}/src/external/minizip/unzip.c"
  "${PROJECT_SOURCE_DIR}/src/external/minizip/mztools.c"
  "${PROJECT_SOURCE_DIR}/src/external/json-parser/*.c"
  "${PROJECT_SOURCE_DIR}/src/external/ddnet/*.cpp"
  "${PROJECT_SOURCE_DIR}/src/shared/*.cpp"
  "${PROJECT_SOURCE_DIR}/src/shared/system/*.cpp"
  "${PROJECT_SOURCE_DIR}/src/shared/tl/*.cpp"
  "${PROJECT_SOURCE_DIR}/src/shared/assets/*.cpp"
  "${PROJECT_SOURCE_DIR}/src/shared/components/*.cpp"
  "${PROJECT_SOURCE_DIR}/src/shared/math/*.cpp"
  "${PROJECT_SOURCE_DIR}/src/shared/geometry/*.cpp"
  "${PROJECT_SOURCE_DIR}/src/shared/imageprocessing/*.cpp"
  "${PROJECT_SOURCE_DIR}/src/generated/assets/*.cpp"
)
if(WIN32)
  set(teeuniverse_shared_src ${teeuniverse_shared_src} "${PROJECT_SOURCE_DIR}/src/external/minizip/iowin32.c")
endif()

# Build
add_library(teeuniverse_shared ${teeuniverse_shared_src})
target_include_directories(teeuniverse_shared PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/src)

target_link_libraries (teeuniverse_shared PUBLIC
  ZLIB::ZLIB
  ICU::i18n
  ICU::uc
)
add_custom_command(TARGET teeuniverse_shared POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data ${DATA_OUTPUT_DIRECTORY})

########################################################################
# CLIENT
########################################################################

if(SDL2_FOUND AND OPENGL_FOUND AND FREETYPE_FOUND)
  # Sources
  file(GLOB teeuniverse_client_src
    "${PROJECT_SOURCE_DIR}/src/external/pnglite/*.c"
    "${PROJECT_SOURCE_DIR}/src/client/gui/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/client/components/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/client/components/*.h"
    "${PROJECT_SOURCE_DIR}/src/client/*.cpp"
  )

  # Build
  add_library(teeuniverse_client ${teeuniverse_client_src})
  target_link_libraries (teeuniverse_client PUBLIC
      teeuniverse_shared
      SDL2::SDL2
      OpenGL::GL
      OpenGL::GLU
      Freetype::Freetype
  )

  if(harfbuzz_FOUND)
    target_link_libraries (teeuniverse_client PUBLIC
      harfbuzz::harfbuzz
      harfbuzz::icu
    )
    target_compile_definitions(teeuniverse_client PRIVATE HARFBUZZ_ENABLED)
  endif()

  ########################################################################
  # GAME
  ########################################################################

  # Sources
  #file(GLOB teeuniverse_game_src
  #  "${PROJECT_SOURCE_DIR}/src/game/gui/*.cpp"
  #  "${PROJECT_SOURCE_DIR}/src/game/components/*.cpp"
  #  "${PROJECT_SOURCE_DIR}/src/game/*.cpp"
  #)

  # Libraries
  #set(teeuniverse_game_libraries
  #  teeuniverse_client
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
    "${PROJECT_SOURCE_DIR}/src/editor/gui/*.h"
    "${PROJECT_SOURCE_DIR}/src/editor/components/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/editor/components/*.h"
    "${PROJECT_SOURCE_DIR}/src/editor/*.cpp"
  )

  # Libraries
  set(teeuniverse_editor_libraries
    teeuniverse_client
  )

  # Build Editor
  add_executable(${TU_EXECNAME_EDITOR} "${PROJECT_SOURCE_DIR}/src/editor.cpp" ${teeuniverse_editor_src})
  target_link_libraries (${TU_EXECNAME_EDITOR} ${teeuniverse_editor_libraries})

  if("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
    set(EDITOR_WIN_ICON_PATH "${PROJECT_SOURCE_DIR}/datasrc/share/pixmaps/teeuniverse.ico")
    configure_file("src/editor.rc.in" "editor.rc" @ONLY)
    target_sources(${TU_EXECNAME_EDITOR} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/editor.rc")

    set_target_properties(${TU_EXECNAME_EDITOR}
        PROPERTIES
            WIN32_EXECUTABLE TRUE
    )
  endif()

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
#  teeuniverse_shared
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

function(set_target_output target dir)
  if(BUILD_TYPE)
    set_target_properties(
      ${target}
      PROPERTIES
      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${dir}"
      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${dir}"
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${dir}"
    )
  else()
    foreach(CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES})
      string(TOUPPER ${CONFIG_TYPE} CONFIG_TYPE)
      set_target_properties(
        ${target}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE} "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE}}/${dir}"
        LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE} "${CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE}}/${dir}"
        RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE}}/${dir}"
      )
    endforeach(CONFIG_TYPE)
  endif()
endfunction()

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

# TUP Info
add_executable(tupinfo ${PROJECT_SOURCE_DIR}/src/tupinfo.cpp)
set_target_output(tupinfo "tupinfo")
target_link_libraries(
  tupinfo
  teeuniverse_tools
)

# Make Assets
function(make_assets progname)
  add_executable(make_assets_${progname} ${PROJECT_SOURCE_DIR}/src/make_assets_${progname}.cpp)
  set_target_output(make_assets_${progname} "make_assets")
  target_link_libraries(
    make_assets_${progname}
    teeuniverse_tools
  )
  add_custom_command(TARGET make_assets_${progname} POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory "${DATA_OUTPUT_DIRECTORY}/packages")
  add_custom_command(TARGET make_assets_${progname} POST_BUILD COMMAND make_assets_${progname} --data-dir "${CMAKE_SOURCE_DIR}/datasrc" --data-dir "${CMAKE_SOURCE_DIR}/data" --save-dir "${DATA_OUTPUT_DIRECTORY}" --save-dir-noinit)

endfunction()

make_assets(gui_editor)
make_assets(teeworlds)
make_assets(ddnet)
make_assets(infclass)
make_assets(openfng)
make_assets(ninslash)
make_assets(sport)

message(STATUS " * tools ready for compilation")

########################################################################
# UNIT TEST
########################################################################

enable_testing()

function(make_test testname)
  add_executable(test_${testname} src/unittest/${testname}.cpp)
  set_target_output(test_${testname} "test")
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

########################################################################
# INSTALL
########################################################################

set(PORTABLE_INSTALL TRUE)
if(PORTABLE_INSTALL)
  install(DIRECTORY "${DATA_OUTPUT_DIRECTORY}" DESTINATION ".")
  if(TARGET ${TU_EXECNAME_EDITOR})
      install(TARGETS ${TU_EXECNAME_EDITOR} DESTINATION ".")
  endif()
  if("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
    install(FILES "other/config_directory.bat" DESTINATION ".")
  endif()
else()
  install(DIRECTORY "${DATA_OUTPUT_DIRECTORY}/languages" DESTINATION "share/teeuniverse")
  install(DIRECTORY "${DATA_OUTPUT_DIRECTORY}/packages" DESTINATION "share/teeuniverse")

  install(DIRECTORY "${DATA_OUTPUT_DIRECTORY}/countryflags" DESTINATION "share/teeuniverse")
  install(DIRECTORY "${DATA_OUTPUT_DIRECTORY}/fonts" DESTINATION "share/teeuniverse")
  if(${PLATFORM} STREQUAL "linux")
    install(FILES "datasrc/share/pixmaps/teeuniverse.png" DESTINATION "share/pixmaps")
  endif()
  if(TARGET ${TU_EXECNAME_EDITOR})
    install(TARGETS ${TU_EXECNAME_EDITOR} DESTINATION bin)
    if(${PLATFORM} STREQUAL "linux")
      install(FILES "datasrc/share/mime/packages/teeuniverse_editor.xml" DESTINATION "share/mime/packages")
      install(FILES "datasrc/share/applications/teeuniverse_editor.desktop" DESTINATION "share/applications" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    endif()
  endif()
endif()

set(CPACK_GENERATOR ZIP)
include(CPack)
