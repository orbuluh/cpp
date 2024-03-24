cmake_minimum_required(VERSION 3.16)

add_executable(simple-server simple-server.cpp ${CapnProtoGenSrcs})
target_link_libraries(simple-server protocol capnp kj kj-async capnp-rpc)
target_include_directories(simple-server PRIVATE ${CMAKE_BINARY_DIR}/protocol)
