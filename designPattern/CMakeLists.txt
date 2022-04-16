set(CMAKE_C_COMPILER    "/usr/bin/clang")
set(CMAKE_CXX_COMPILER  "/usr/bin/clang++")

cmake_minimum_required(VERSION 3.14)
project(designPattern VERSION 0.1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_subdirectory(creational/factory Factory)
add_subdirectory(creational/abstractfactory Abstractfactory)
add_subdirectory(creational/prototype Prototype)
add_subdirectory(structural/adapter Adapter)
add_subdirectory(structural/bridge Bridge)
add_subdirectory(structural/composite Composite)
add_subdirectory(structural/decorator Decorator)
add_subdirectory(structural/facade Facade)
add_subdirectory(structural/flyweight Flyweight)
add_subdirectory(structural/proxy Proxy)
add_subdirectory(behavior/chainofresp Chainofresp)
add_subdirectory(behavior/command Command)
add_subdirectory(behavior/iterator Iterator)
add_subdirectory(behavior/mediator Mediator)
add_subdirectory(behavior/memento Memento)
add_subdirectory(behavior/observer Observer)
add_subdirectory(behavior/state State)
add_subdirectory(behavior/strategy Strategy)
add_subdirectory(behavior/templatemethod Templatemethod)
add_subdirectory(behavior/visitor Visitor)
add_subdirectory(behavior/maybeMonad MaybeMonad)

list(APPEND EXTRA_LIBS
    Factory
    Abstractfactory
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
    Strategy
    Templatemethod
    Visitor
    MaybeMonad
)

add_executable(designPattern main.cpp)

target_link_libraries(designPattern LINK_PUBLIC ${EXTRA_LIBS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
