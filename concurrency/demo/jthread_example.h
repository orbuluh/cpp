#pragma once

// from Concurrency in C++: A Programmer’s Overview (part 1 of 2) - Fedor Pikus
// - CppNow 2022

#include "thread_safe_cout.h"

#include <chrono>
#include <thread>

namespace jthread_example {

void demo() {
  using namespace demo_util;
  std::jthread t1([&] {
    for (size_t i = 0; i < 10; ++i) {
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      ts_cout{} << "<---\n";
    }
  });

  std::jthread t2([&] {
    for (size_t i = 0; i < 10; ++i) {
      std::this_thread::sleep_for(std::chrono::milliseconds(300));
      ts_cout{} << "--->\n";
    }
  });
}

}  // namespace jthread_example
