cmake_minimum_required(VERSION 3.15)
project(AsioInotifyFileWatcher)

# Find the Boost library components you need (e.g., Boost.Asio)
find_package(Boost REQUIRED COMPONENTS system thread)

add_library(file_monitor_service INTERFACE)

target_link_libraries(file_monitor_service INTERFACE Boost::boost Boost::system
                                                     Boost::thread)

target_include_directories(file_monitor_service INTERFACE .)
