#pragma once

// from Concurrency in C++: A Programmer’s Overview (part 1 of 2) - Fedor Pikus
// - CppNow 2022

#include "thread_safe_cout.h"

#include <barrier>
#include <chrono>
#include <thread>

namespace barrier_example {

void demo() {
  using namespace demo_util;
  std::atomic<int> done_heartbeat = 0;
  std::jthread heartbeat([&] {
    while (!done_heartbeat) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      ts_cout{} << "...hb...\n";
    }
  });
  std::barrier b(2);
  auto work = [&](const char* s, bool wait) {
    ts_cout{} << "starting thread..." << s << "\n";
    b.arrive_and_wait();
    ts_cout{} << "starting work on thread after 2 threads reach barrier..." << s << "\n";

    if (wait) {
      ts_cout{} << "going to sleep on thread..." << s << "\n";
      std::this_thread::sleep_for(std::chrono::seconds(5));
      ts_cout{} << "woke on thread..." << s << "\n";
    }
    b.arrive_and_wait();
    ts_cout{} << "Done work on thread after reach 2nd barrier..." << s << "\n";
  };
  {
    ts_cout{} << "Creating thread t1...\n";
    std::jthread t1(work, "t1", true);
    ts_cout{} << "Created thread t1...\n";
    std::this_thread::sleep_for(std::chrono::seconds(5));
    ts_cout{} << "Creating thread t2...\n";
    std::jthread t2(work, "t2", false);
    ts_cout{} << "Created thread t2...\n";
  }
  done_heartbeat = 1;
}

}  // namespace barrier_example
