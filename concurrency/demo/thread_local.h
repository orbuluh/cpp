#pragma once

// from Concurrency in C++: A Programmer’s Overview (part 1 of 2) - Fedor Pikus
// - CppNow 2022

#include <iostream>
#include <thread>

namespace thread_local_example {

struct S {
  S() {
    std::cout << "Constructing on thread " << std::this_thread::get_id()
              << '\n';
  }
  ~S() {
    std::cout << "Destructing on thread " << std::this_thread::get_id() << '\n';
  }
};

thread_local S s;

int* f() {
  thread_local int i{};
  return &i;
}

void demo() {
  {
    S* p = &s;
    std::cout << "Main thread: " << std::this_thread::get_id() << " s@" << p
              << " f() return @" << f() << '\n';
  }

  std::jthread t1([&] {
    S* p = &s;
    std::cout << "Worker thread: " << std::this_thread::get_id() << " s@" << p
              << " f() return @" << f() << '\n';
  });
}

}  // namespace thread_local_example
