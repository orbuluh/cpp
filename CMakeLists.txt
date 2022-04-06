set(CMAKE_C_COMPILER    "/usr/bin/clang")
set(CMAKE_CXX_COMPILER  "/usr/bin/clang++")

cmake_minimum_required(VERSION 3.14)
project(PlayGround VERSION 0.1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#--------------GTest--------------
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
#--------------boost--------------
find_package(Boost 1.78.0 COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
#---------------------------------
add_subdirectory(designPattern/4.prototype Prototype)
add_subdirectory(designPattern/6.adapter Adapter)
add_subdirectory(designPattern/7.bridge Bridge)
add_subdirectory(designPattern/8.composite Composite)
add_subdirectory(designPattern/9.decorator Decorator)
add_subdirectory(designPattern/10.facade Facade)
add_subdirectory(designPattern/11.flyweight Flyweight)
add_subdirectory(designPattern/12.proxy Proxy)
add_subdirectory(designPattern/13.chainofresp Chainofresp)
add_subdirectory(designPattern/14.command Command)
add_subdirectory(designPattern/16.iterator Iterator)
add_subdirectory(designPattern/17.mediator Mediator)
add_subdirectory(designPattern/18.memento Memento)
add_subdirectory(designPattern/20.observer Observer)
add_subdirectory(designPattern/21.state State)
add_subdirectory(designPattern/25.maybeMonad MaybeMonad)
add_subdirectory(boost BoostCookBook)

list(APPEND EXTRA_LIBS
    Prototype
    Adapter
    Bridge
    Composite
    Decorator
    Facade
    State
    Flyweight
    Proxy
    Chainofresp
    Command
    Memento
    Iterator
    Mediator
    Observer
    MaybeMonad
)

list(APPEND EXTRA_LIBS BoostCookBook)
list(APPEND EXTRA_LIBS gtest_main gtest)
list(APPEND EXTRA_LIBS ${Boost_LIBRARIES})
#list(APPEND EXTRA_LIBS Boost::program_options)
#list(APPEND EXTRA_LIBS Boost::any)


add_executable(PlayGround main.cpp)

target_link_libraries(PlayGround LINK_PUBLIC ${EXTRA_LIBS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

include(CPack)
include(GoogleTest)
include(CTest)
gtest_discover_tests(PlayGround)