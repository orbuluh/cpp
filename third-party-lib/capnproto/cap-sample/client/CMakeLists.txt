cmake_minimum_required(VERSION 3.16)

add_executable(client main.cpp ${CapnProtoGenSrcs} src/sample_client.cpp)

target_include_directories(client PUBLIC src)
target_link_libraries(client cap_sample_interface capnp kj kj-async capnp-rpc)
target_include_directories(client PRIVATE ${CMAKE_BINARY_DIR}/interface)
