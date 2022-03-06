cmake_minimum_required(VERSION 3.0.0)
project(PlayGround VERSION 0.1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(designPattern/25.maybeMonad MaybeMonad)

list(APPEND EXTRA_LIBS MaybeMonad Interpreter)

add_executable(PlayGround main.cpp)

target_link_libraries(PlayGround PUBLIC ${EXTRA_LIBS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

include(CPack)