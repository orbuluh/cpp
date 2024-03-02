cmake_minimum_required(VERSION 3.16)

project(cap-sample CXX)

find_package(CapnProto CONFIG REQUIRED)

add_subdirectory(interface)
add_subdirectory(client)
add_subdirectory(server)
