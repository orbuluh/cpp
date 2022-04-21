set(CMAKE_C_COMPILER    "/usr/bin/clang")
set(CMAKE_CXX_COMPILER  "/usr/bin/clang++")

cmake_minimum_required(VERSION 3.14)
project(GTestExample VERSION 0.1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#--------------GTest--------------
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

list(APPEND EXTRA_LIBS gtest_main gtest)

add_executable(GTestExample main.cpp)
target_link_libraries(GTestExample LINK_PUBLIC ${EXTRA_LIBS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "playground")

include(CPack)
include(GoogleTest)
include(CTest)
gtest_discover_tests(GTestExample)