cmake_minimum_required(VERSION 3.6)
project(tbb_hello)

find_package(TBB REQUIRED)

message(STATUS "Using Protocol Buffers ${protobuf_VERSION}")

#set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

#set(PROTO_SOURCES proto/message.proto)
#set(PROJECT_NAME cmake_protobuf)
#set(executable_name ${PROJECT_NAME})

add_executable(tbb_hello main.cpp)
target_link_libraries(tbb_hello TBB::tbb)