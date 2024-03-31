cmake_minimum_required(VERSION 3.16)

project(cap-sample CXX)

find_package(CapnProto CONFIG REQUIRED)

add_subdirectory(protocol)

add_subdirectory(simple-client)

add_subdirectory(simple-server)
