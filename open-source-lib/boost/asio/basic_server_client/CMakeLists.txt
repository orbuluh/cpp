cmake_minimum_required(VERSION 3.15)
project(MyProject)

# Find the Boost library components you need (e.g., Boost.Asio)
find_package(Boost REQUIRED COMPONENTS system)

# Define the utility library
add_library(utility_lib STATIC
    util/utility.cpp
)

# Include directories for the utility library
target_include_directories(utility_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/util
)

# Define the server executable
add_executable(server
    server.cpp
)

# Link the utility library to the server executable
target_link_libraries(server PRIVATE
    utility_lib
    Boost::system
)

# Define the client executable
add_executable(client
    client.cpp
)

# Link the utility library to the client executable
target_link_libraries(client PRIVATE
    utility_lib
    Boost::system
)