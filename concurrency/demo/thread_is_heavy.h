#pragma once

// from Concurrency in C++: A Programmer’s Overview (part 1 of 2) - Fedor Pikus
// - CppNow 2022

#include <chrono>
#include <thread>

namespace thread_is_heavy {

void demo() {
  size_t N;
  std::cout << "number of threads: ";
  std::cin >> N;
  double t_start = 0, t_join = 0;
  double t_start_max = 0, t_join_max = 0;
  for (size_t i = 0; i < N; ++i) {
    auto start = std::chrono::system_clock::now();
    auto thread_start = start;
    std::thread t([&]() { thread_start = std::chrono::system_clock::now(); });
    t.join();
    auto end = std::chrono::system_clock::now();
    double t1 = std::chrono::duration<double>(end - start).count();
    double t2 = std::chrono::duration<double>(thread_start - start).count();
    t_join += t1;
    t_start += t2;
    t_join_max = std::max(t_join_max, t1);
    t_start_max = std::max(t_start_max, t2);

    std::cout << "Ave time to start a thread: " << (t_start / N)
              << "s, to join: " << (t_join / N) << "s\n";
    std::cout << "Max time to start a thread: " << (t_start_max / N)
              << "s, to join: " << (t_join_max / N) << "s\n";
  }
}

}  // namespace thread_is_heavy
