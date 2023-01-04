cmake_minimum_required(VERSION 3.6)
project(cmake_protobuf)

find_package(protobuf REQUIRED)

message(STATUS "Using Protocol Buffers ${protobuf_VERSION}")

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

set(PROTO_SOURCES proto/message.proto)
set(PROJECT_NAME cmake_protobuf)

set(executable_name ${PROJECT_NAME})

add_executable(${executable_name} main.cpp ${PROTO_SOURCES})
target_link_libraries(${executable_name} protobuf::libprotobuf)
protobuf_generate(TARGET ${executable_name})