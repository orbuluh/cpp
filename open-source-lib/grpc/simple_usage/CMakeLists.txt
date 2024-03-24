cmake_minimum_required(VERSION 3.13)

project(grpc_greeter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)

set(PROTO_PATH "${CMAKE_CURRENT_SOURCE_DIR}/protos")
set(GENERATED_PROTOBUF_PATH "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY ${GENERATED_PROTOBUF_PATH})

# Generate protobuf files
get_filename_component(PROTO "${PROTO_PATH}/greeter.proto" ABSOLUTE)
get_filename_component(PROTO_DIR "${PROTO}" PATH)

set(PROTO_SRCS "${GENERATED_PROTOBUF_PATH}/greeter.pb.cc")
set(PROTO_HDRS "${GENERATED_PROTOBUF_PATH}/greeter.pb.h")
set(GRPC_SRCS "${GENERATED_PROTOBUF_PATH}/greeter.grpc.pb.cc")
set(GRPC_HDRS "${GENERATED_PROTOBUF_PATH}/greeter.grpc.pb.h")

add_custom_command(
  OUTPUT "${PROTO_SRCS}" "${PROTO_HDRS}" "${GRPC_SRCS}" "${GRPC_HDRS}"
  COMMAND protobuf::protoc
  ARGS "--grpc_out=${GENERATED_PROTOBUF_PATH}"
       "--cpp_out=${GENERATED_PROTOBUF_PATH}"
       "-I${PROTO_DIR}"
       "--plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin"
       "${PROTO}"
  DEPENDS "${PROTO}"
)

add_library(my_proto_lib ${PROTO_SRCS} ${PROTO_HDRS} ${GRPC_SRCS} ${GRPC_HDRS})
target_include_directories(my_proto_lib PUBLIC ${GENERATED_PROTOBUF_PATH})
target_link_libraries(my_proto_lib PUBLIC protobuf::libprotobuf gRPC::grpc++)

# Server
add_executable(server src/server.cpp)
target_link_libraries(server PRIVATE my_proto_lib)

# Client
add_executable(client src/client.cpp)
target_link_libraries(client PRIVATE my_proto_lib)