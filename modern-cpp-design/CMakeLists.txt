cmake_minimum_required(VERSION 3.14...3.28)

# Project name and a few useful settings. Other commands can pick up the results

project(
  ModernCMakeExample
  VERSION 0.1
  DESCRIPTION "An example project with CMake"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

# Only do these if this is the main project, and not if it is included through
# add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  # Optionally set things like CMAKE_CXX_STANDARD,
  # CMAKE_POSITION_INDEPENDENT_CODE here

  # Let's ensure -std=c++xx instead of -std=g++xx This means that CMake will
  # explicitly request the C++ standard version specified by xx without allowing
  # any compiler-specific extensions that may deviate from the standard. By
  # disabling compiler-specific extensions (CMAKE_CXX_EXTENSIONS OFF),
  # developers can ensure that their codebase adheres more closely to standard
  # C++ practices, improving portability, maintainability, and readability.
  set(CMAKE_CXX_EXTENSIONS OFF)

  # Let's nicely support folders in IDEs
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  # Testing only available if this is the main app Note this needs to be done in
  # the main CMakeLists since it calls enable_testing, which must be in the main
  # CMakeLists
  include(CTest)

  # Docs only available if this is the main app

  find_package(Doxygen)
  if(Doxygen_FOUND)
    add_subdirectory(docs)
  else()
    message(STATUS "Doxygen not found, not building docs")
  endif()
endif()

add_subdirectory(util)
add_subdirectory(ch3_typelists)

# The executable code is here

add_subdirectory(apps)

# Testing only available if this is the main app Emergency override
# MODERN_CMAKE_BUILD_TESTING provided as well

if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING)
   AND BUILD_TESTING)
  add_subdirectory(tests)
endif()
