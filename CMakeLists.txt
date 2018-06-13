cmake_minimum_required(VERSION 3.9)
project(ape CXX)

set(CMAKE_BUILD_TYPE Debug)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries)

# Generate version information

set(APE_VERSION "DONTRELEASEME")
find_package(Git)
if(GIT_FOUND)
  execute_process(
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND ${GIT_EXECUTABLE} status
    RESULT_VARIABLE IS_GIT_REPO
    OUTPUT_QUIET)

  if (IS_GIT_REPO EQUAL 0)
    # Adadpted from Dolphin
    # Make sure version information gets re-run when the current Git HEAD changes
    execute_process(
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
      COMMAND ${GIT_EXECUTABLE} rev-parse --git-path HEAD
      OUTPUT_VARIABLE APE_GIT_HEAD
      OUTPUT_STRIP_TRAILING_WHITESPACE)

    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${APE_GIT_HEAD}")

    execute_process(
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
      COMMAND ${GIT_EXECUTABLE} rev-list HEAD --count
      OUTPUT_VARIABLE APE_BUILD_NUMBER
      OUTPUT_STRIP_TRAILING_WHITESPACE)

    execute_process(
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
      COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
      OUTPUT_VARIABLE APE_BRANCH
      OUTPUT_STRIP_TRAILING_WHITESPACE)

    execute_process(
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
      COMMAND ${GIT_EXECUTABLE} diff --shortstat HEAD
      OUTPUT_VARIABLE IS_GIT_DIRTY
      OUTPUT_STRIP_TRAILING_WHITESPACE)

    if (NOT APE_BRANCH STREQUAL "master" AND NOT APE_BRANCH STREQUAL "stable")
      set(APE_VERSION "${APE_VERSION}-${APE_BRANCH}")
    endif()

    if (NOT APE_BRANCH STREQUAL "stable" OR NOT IS_GIT_DIRTY STREQUAL "")
      set(APE_VERSION "${APE_VERSION}-${APE_BUILD_NUMBER}")
    endif()

    if (NOT IS_GIT_DIRTY STREQUAL "")
      set(APE_VERSION "${APE_VERSION}-dirty")
    endif()

  endif()
endif()

include_directories(Source)

# Documentation
find_package(Doxygen)

if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Docs/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
  add_custom_target(Docs ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Turn all warnings on
if (MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

# C++ version file
configure_file(Source/Version.h.in ${CMAKE_CURRENT_BINARY_DIR}/Version.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Add components
add_subdirectory(Source/Common)
add_subdirectory(Source/Core)

add_subdirectory(Source/Ape)
add_subdirectory(Source/Tools)
add_subdirectory(Source/Tests)
