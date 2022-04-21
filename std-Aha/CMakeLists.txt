set(CMAKE_C_COMPILER    "/usr/bin/clang")
set(CMAKE_CXX_COMPILER  "/usr/bin/clang++")

cmake_minimum_required(VERSION 3.14)
project(PlayGround VERSION 0.1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(PlayGround main.cpp)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "playground")