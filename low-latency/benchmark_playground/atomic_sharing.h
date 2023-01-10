#pragma once

#include <benchmark/benchmark.h>

#include <thread>
#include <atomic>

// from Concurrency in C++: A Programmer’s Overview (part 2 of 2) - Fedor Pikus

namespace atomic_sharing {

#define REPEAT2(x) \
  {x} { x }
#define REPEAT4(x) REPEAT2(x) REPEAT2(x)
#define REPEAT8(x) REPEAT4(x) REPEAT4(x)
#define REPEAT16(x) REPEAT8(x) REPEAT8(x)
#define REPEAT32(x) REPEAT16(x) REPEAT16(x)
#define REPEAT64(x) REPEAT32(x) REPEAT32(x)
#define REPEAT(x) REPEAT64(x)

//------------------------------------------------------------
// (C++17 feature to find L1 cache size)
// https://en.cppreference.com/w/cpp/thread/hardware_destructive_interference_size
#ifdef __cpp_lib_hardware_interference_size
using std::hardware_constructive_interference_size;
using std::hardware_destructive_interference_size;
#else
// 64 bytes on x86-64 │ L1_CACHE_BYTES │ L1_CACHE_SHIFT │ __cacheline_aligned │
// ...
constexpr std::size_t hardware_constructive_interference_size = 64;
constexpr std::size_t hardware_destructive_interference_size = 64;
#endif

std::atomic<long> a[1024] = {};

static void BM_Shared(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(a[0].fetch_add(1));
  }
}

static void BM_FalseShared(benchmark::State& state) {
  const int i = state.thread_index();
    // each thread access different atomic
    // C++ guarantee that accessing on different object is always thread-safe.
    // (regardless if it's atomic)
  for (auto _ : state) {
    benchmark::DoNotOptimize(a[i].fetch_add(1));
  }
}

static void BM_NotShared(benchmark::State& state) {
  // keep the access far enough
  const int i = state.thread_index() * hardware_destructive_interference_size /
                sizeof(a[0]);
  for (auto _ : state) {
    benchmark::DoNotOptimize(a[i].fetch_add(1));
  }
}

const int max_threads = std::thread::hardware_concurrency();

#define ARGS ->ThreadRange(1, max_threads)->UseRealTime()

BENCHMARK(BM_Shared) ARGS;
BENCHMARK(BM_FalseShared) ARGS;
BENCHMARK(BM_NotShared) ARGS;

}  // namespace atomic_sharing