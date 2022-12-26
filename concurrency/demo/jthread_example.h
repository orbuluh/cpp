#pragma once

// from Concurrency in C++: A Programmer’s Overview (part 1 of 2) - Fedor Pikus
// - CppNow 2022

#include <chrono>
#include <thread>

namespace jthread_example {

void demo() {
  std::mutex m;
  std::jthread t1([&] {
    for (size_t i = 0; i < 10; ++i) {
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      {
        std::lock_guard _{m};
        std::cout << "<---\n";
      }
    }
  });

  std::jthread t2([&] {
    for (size_t i = 0; i < 10; ++i) {
      std::this_thread::sleep_for(std::chrono::milliseconds(300));
      {
        std::lock_guard _{m};
        std::cout << "--->\n";
      }
    }
  });
}

}  // namespace jthread_example
