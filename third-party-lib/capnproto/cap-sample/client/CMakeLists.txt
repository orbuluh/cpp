cmake_minimum_required(VERSION 3.16)

find_package(CapnProto CONFIG REQUIRED)

capnp_generate_cpp(CapnProtoGenSrcs CapnProtoGenHdrs interface/sample.capnp)

add_executable(client main.cpp ${CapnProtoGenSrcs} src/sample_client.cpp)

target_include_directories(client PUBLIC src)
target_link_libraries(client capnp kj kj-async capnp-rpc)
target_include_directories(client PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
