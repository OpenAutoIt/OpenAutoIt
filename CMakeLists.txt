# OpenAutoIt

cmake_minimum_required(VERSION 3.10.2)

# Set version
set(OPENAUT_VERSION_MAJOR 0)
set(OPENAUT_VERSION_MINOR 1)
set(OPENAUT_VERSION_PATCH 0)

# Get git info
execute_process(
  COMMAND git log --format="%H" -n 1
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE OPENAUT_GIT_SHA_FULL
  OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(
  COMMAND git log --format="%h" -n 1
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE OPENAUT_GIT_SHA_SHORT
  OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(
  COMMAND git branch --show-current
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE OPENAUT_GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE)

# Get uname info
execute_process(
  COMMAND uname -s -r -m -o
  OUTPUT_VARIABLE OPENAUT_UNAME
  OUTPUT_STRIP_TRAILING_WHITESPACE)

# Project
project(
  "OpenAutoIt"
  LANGUAGES CXX
  VERSION ${OPENAUT_VERSION_MAJOR}.${OPENAUT_VERSION_MINOR}.${OPENAUT_VERSION_PATCH})

# Dependencies
add_subdirectory(external)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/Phi/scripts/cmake)

include(Environment)
include(Testing)
include(CTest)
include(ProjectOptions)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(DEFINED PHI_PLATFORM_EMSCRIPTEN OR (DEFINED PHI_PLATFORM_WINDOWS AND NOT DEFINED
                                                                        PHI_COMPILER_MSVC))
  set(_lto_default OFF)
else()
  set(_lto_default ON)
endif()

# Needed for MSVC to build without errors
if(PHI_PLATFORM_WINDOWS)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

option(OA_BUILD_TOOLS "" ON)
option(OA_LTO "" ${_lto_default})
option(OA_COVERAGE_BUILD "" OFF)
option(OA_ENABLE_SANITIZER_ADDRESS "" OFF)
option(OA_ENABLE_SANITIZER_LEAK "" OFF)
option(OA_ENABLE_SANITIZER_MEMORY "" OFF)
option(OA_ENABLE_SANITIZER_THREAD "" OFF)
option(OA_ENABLE_SANITIZER_UNDEFINED "" OFF)
option(OA_FUZZING_BUILD "" OFF)

# Targets
add_subdirectory("parser")

add_subdirectory("runtime")

add_subdirectory("test_runner")

add_subdirectory("fuzzing")

if(${OA_BUILD_TOOLS})
  add_subdirectory("tools")
endif()

add_subdirectory("OpenAutoIt")

if(${OA_LTO})
  set(_extra ${_extra} LTO)
endif()

if(${OA_COVERAGE_BUILD})
  message(STATUS "Enabling coverage info generation")

  set(_extra ${_extra} COVERAGE)
endif()

# Configure sanitizers
set(_list_of_sanitizers)
if(${OA_ENABLE_SANITIZER_ADDRESS})
  message(STATUS "Enabling address sanitizer")

  set(_list_of_sanitizers "address")
endif()
if(${OA_ENABLE_SANITIZER_LEAK})
  message(STATUS "Enabling leak sanitizer")

  set(_list_of_sanitizers ${_list_of_sanitizers} "leak")
endif()
if(${OA_ENABLE_SANITIZER_MEMORY})
  message(STATUS "Enabling memory sanitizer")

  set(_list_of_sanitizers ${_list_of_sanitizers} "memory")
endif()
if(${OA_ENABLE_SANITIZER_THREAD})
  message(STATUS "Enabling thread sanitizer")

  set(_list_of_sanitizers ${_list_of_sanitizers} "thread")
endif()
if(${OA_ENABLE_SANITIZER_UNDEFINED})
  message(STATUS "Enabling undefined sanitizer")

  set(_list_of_sanitizers ${_list_of_sanitizers} "undefined")
endif()

if(_list_of_sanitizers)
  set(_extra ${_extra} SANITIZER ${_list_of_sanitizers})
endif()

if(${OA_FUZZING_BUILD})
  set(_extra FUZZING_BUILD)
endif()

# Configure
phi_configure_project(
  DEBUG_FLAGS
  OPTIMIZATION_FLAGS
  WARNINGS
  WARNINGS_AS_ERRORS
  PEDANTIC
  NO_EXCEPTIONS
  STANDARD
  "latest"
  EXTERNAL
  "external"
  ${_extra})
