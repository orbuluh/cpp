cmake_minimum_required(VERSION 3.14...3.28)

# Project name and a few useful settings. Other commands can pick up the results

project(
  asio_chat_demo
  VERSION 0.1
  DESCRIPTION "An example project with CMake"
  LANGUAGES CXX)

include(FetchContent)
find_package(Boost REQUIRED)

add_executable(chat_server chat_server.cpp)
target_link_libraries(chat_server boost_system boost_thread)

add_executable(chat_client chat_client.cpp)
target_link_libraries(chat_client boost_system boost_thread)
