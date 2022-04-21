set(CMAKE_C_COMPILER    "/usr/bin/clang")
set(CMAKE_CXX_COMPILER  "/usr/bin/clang++")

cmake_minimum_required(VERSION 3.14)
project(templateMeta VERSION 0.1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#add_subdirectory(ch2.managing_threads ManagingThreads)
#
#list(APPEND EXTRA_LIBS
#    ManagingThreads
#)

add_executable(templateMeta main.cpp)

target_link_libraries(templateMeta LINK_PUBLIC ${EXTRA_LIBS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "playground")