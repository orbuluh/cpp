cmake_minimum_required(VERSION 3.14)

project(project_name)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/release")

enable_testing()

add_subdirectory(src)
add_subdirectory(util)
add_subdirectory(test)