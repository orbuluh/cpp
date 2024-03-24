cmake_minimum_required(VERSION 3.15)
project(AsioInotifyFileWatcher)

# Find the Boost library components you need (e.g., Boost.Asio)
find_package(Boost REQUIRED COMPONENTS system)

add_subdirectory(file_monitor_service)

add_executable(asio_file_watcher main.cpp)

target_link_libraries(asio_file_watcher PRIVATE file_monitor_service)
target_include_directories(asio_file_watcher PRIVATE .)
