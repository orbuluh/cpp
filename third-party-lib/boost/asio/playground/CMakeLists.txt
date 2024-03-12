cmake_minimum_required(VERSION 3.15)
project(MyProject)

# Find the Boost library components you need (e.g., Boost.Asio)
find_package(Boost REQUIRED COMPONENTS system)

add_subdirectory(util)
add_subdirectory(cppcon_2016_asio_talks)

add_executable(playground main.cpp)

target_link_libraries(playground PRIVATE cppcon_2016_asio_talks)
