cmake_minimum_required(VERSION 3.6)
project(tbb_hello)

find_package(TBB REQUIRED)

add_executable(tbb_hello main.cpp)
target_link_libraries(tbb_hello TBB::tbb)