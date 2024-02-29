cmake_minimum_required(VERSION 3.16)

find_package(CapnProto CONFIG REQUIRED)

capnp_generate_cpp(CapnProtoGenSrcs CapnProtoGenHdrs interface/sample.capnp)

add_executable(server main.cpp ${CapnProtoGenSrcs} src/rpc_event.cpp
                      src/sample_server.cpp)

target_include_directories(server PUBLIC src)
target_link_libraries(server capnp kj kj-async capnp-rpc)
target_include_directories(server PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
