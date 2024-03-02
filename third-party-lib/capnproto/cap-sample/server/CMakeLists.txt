cmake_minimum_required(VERSION 3.16)

add_executable(server main.cpp src/rpc_event.cpp src/sample_server.cpp)

target_include_directories(server PUBLIC src)
target_link_libraries(server cap_sample_interface capnp kj kj-async capnp-rpc)

target_include_directories(server PRIVATE ${CMAKE_BINARY_DIR}/interface)
