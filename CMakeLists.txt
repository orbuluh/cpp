cmake_minimum_required(VERSION 3.14)
project(PlayGround VERSION 0.1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_subdirectory(designPattern/20.observer Observer)
add_subdirectory(designPattern/25.maybeMonad MaybeMonad)

list(APPEND EXTRA_LIBS MaybeMonad Observer)

add_executable(PlayGround main.cpp)
target_link_libraries(PlayGround PUBLIC ${EXTRA_LIBS} gtest_main gtest)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

include(CPack)
include(GoogleTest)
include(CTest)
gtest_discover_tests(PlayGround)