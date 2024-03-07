cmake_minimum_required(VERSION 3.16)

add_executable(simple-client simple-client.cpp ${CapnProtoGenSrcs})

target_link_libraries(simple-client protocol capnp kj kj-async capnp-rpc)
target_include_directories(simple-client PRIVATE ${CMAKE_BINARY_DIR}/protocol)
