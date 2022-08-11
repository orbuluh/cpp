set(CMAKE_C_COMPILER    "/usr/bin/clang")
set(CMAKE_CXX_COMPILER  "/usr/bin/clang++")

cmake_minimum_required(VERSION 3.14)
project(cppIdioms VERSION 0.1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#add_subdirectory(code CppIdioms)
#add_subdirectory(code)

#list(APPEND EXTRA_LIBS CppIdioms)

add_executable(playground main.cpp)

#target_link_libraries(cppIdioms LINK_PUBLIC ${EXTRA_LIBS})
target_link_libraries(playground LINK_PUBLIC)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
