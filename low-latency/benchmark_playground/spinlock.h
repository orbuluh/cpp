// from Lightning Talk: A Spinlock Implementation - Fedor Pikus - CppNow 2022
//
// [2] is the normal spinlock condition, and naive implementation will just use
//     a while loop on this condition. So you exchange whatever in the atomic
//     with 1, and supposed return value is 1, it means somebody already lock
//     and holding it. But if return value is 0, you must be the first one
//     locking it. And to release it, you simply write 0 into it. You don't need
//     any read or so, because you are the only one that could have locked it.
//
// [1] is the trick. Note that the condition in [2] is slow, because exchange
//     is a read-write operation. And write operation requires exclusive lock
//     on the cacheline. This is the part that is slow. Hence we have the
//     condition in [1], where we read and see if it has been locked. So because
//     it's only a read operation, when it's locked, it's no longer locking the
//     whole cache line. (Basically like a double locking mechanism.)
//
// [3] is the other trick when scheduler just prefer to run your if loop rather
//     than letting someone else to unlock it. Although because of certain
//     fairness mechanism in OS, unlock would still get its time slice
//     eventually, it will still just reduce the throughput. So the nanosleep
//     force scheduler to give up on you. Using 8 is the black magic through
//     experiments and benchmarking. (There are some other way than nanosleep,
//     but still per experiments, not better than nanosleep)
//
// [4] could change with exponential back-off, which Fidor didn't want to
//     confuse listener in the talk

#pragma once

#include <time.h>
#include <atomic>

class spinlock {
 public:
  spinlock() : flag_(0) {}
  void lock() {
    static const timespec ns = {0, 1}; // *[4]
    for (int i = 0; flag_.load(std::memory_order_relaxed) ||  // *[2]
                    flag_.exchange(1, std::memory_order_acquire); // *[1]
         ++i) {
      if (i == 8) { // *[3]
        i = 0;
        nanosleep(&ns, NULL);
      }
    }
  }
  void unlock() { flag_.store(0, std::memory_order_release); } // *[2]

 private:
  std::atomic<unsigned int> flag_;
};
