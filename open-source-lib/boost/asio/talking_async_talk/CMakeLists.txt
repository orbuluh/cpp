cmake_minimum_required(VERSION 3.15)
project(talking_async)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# add_definitions(-DASIO_ENABLE_HANDLER_TR)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fno-inline")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_ASIO_ENABLE_HANDLER_TRACKING")

find_package(Boost REQUIRED COMPONENTS system)

add_library(common_lib common/common.cpp)

set(FILENAMES "")

foreach(NUMBER RANGE 0 7)
  list(APPEND FILENAMES "step_${NUMBER}")
endforeach()

foreach(FILENAME IN LISTS FILENAMES)
  add_executable(${FILENAME} ${FILENAME}.cpp)
  target_link_libraries(${FILENAME} common_lib)
  target_include_directories(${FILENAME} PRIVATE ${PROJECT_SOURCE_DIR})
endforeach()
