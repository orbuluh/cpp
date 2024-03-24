set(CMAKE_C_COMPILER    "/usr/bin/clang")
set(CMAKE_CXX_COMPILER  "/usr/bin/clang++")

cmake_minimum_required(VERSION 3.14)
project(BoostDemo VERSION 0.1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#--------------boost--------------
# (Your system need to install boost so CMake can find the package here. It's
#  not download something automatically and install for you here!)
find_package(Boost 1.78.0 COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
list(APPEND EXTRA_LIBS ${Boost_LIBRARIES})
#------------------------------------------
list(APPEND EXTRA_FILES any/any.cpp)
list(APPEND EXTRA_FILES asio/asio.cpp)
list(APPEND EXTRA_FILES msm/meta_state_machine.cpp)
#------------------------------------------

add_executable(BoostDemo main.cpp ${EXTRA_FILES})
target_link_libraries(BoostDemo LINK_PUBLIC ${EXTRA_LIBS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "playground")
