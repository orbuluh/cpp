// Modified from CoffeeBeforeArch's code


#include <benchmark/benchmark.h>

#include <array>
#include <atomic>
#include <thread>
#include <vector>

namespace cache_behavior {

void work(std::atomic<int>& a, int n) {
  for (int i = 0; i < 400000 / n; i++) {
    a++;
  }
}

void allThreadAccessSameAtomic(std::atomic<int>& oneAtomic, int n) {
  // each thread writes to its own atomics, however, because of false sharing
  // data might be bouncing around different cores.
  std::vector<std::jthread> threads(n);
  for (int i = 0; i < n; ++i) {
    threads[i] = std::jthread([&]() { work(oneAtomic, n); });
  }
}

static void BM_directSharing(benchmark::State& state) {
  std::atomic<int> oneAtomic;
  for (auto _ : state) {
    allThreadAccessSameAtomic(oneAtomic, state.range(0));
  }
}

BENCHMARK(BM_directSharing)
    ->Name("DirectSharing_test_with_#_threads")
    ->RangeMultiplier(2)
    ->Range(1, 8)
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);

//------------------------------------------------------------

void eachThreadHasOwnAtomic(std::vector<std::atomic<int>>& atomics, int n) {
  // each thread writes to its own atomics, however, because of false sharing
  // data might be bouncing around different cores.
  std::vector<std::jthread> threads(n);
  for (int i = 0; i < n; ++i) {
    threads[i] = std::jthread([&]() { work(atomics[i], n); });
  }
}

static void BM_falseSharing(benchmark::State& state) {
  const int n = state.range(0);
  std::vector<std::atomic<int>> atomics(n);
  for (auto _ : state) {
    eachThreadHasOwnAtomic(atomics, n);
  }
}

BENCHMARK(BM_falseSharing)
    ->Name("FalseSharing_test_with_#_threads")
    ->RangeMultiplier(2)
    ->Range(1, 8)
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);

//------------------------------------------------------------
// (C++17 feature to find L1 cache size)
// https://en.cppreference.com/w/cpp/thread/hardware_destructive_interference_size
#ifdef __cpp_lib_hardware_interference_size
    using std::hardware_constructive_interference_size;
    using std::hardware_destructive_interference_size;
#else
    // 64 bytes on x86-64 │ L1_CACHE_BYTES │ L1_CACHE_SHIFT │ __cacheline_aligned │ ...
    constexpr std::size_t hardware_constructive_interference_size = 64;
    constexpr std::size_t hardware_destructive_interference_size = 64;
#endif

// can also run command `getconf LEVEL1_DCACHE_LINESIZE` to check L1 cache size

struct alignas(hardware_constructive_interference_size) AlignedAtomic {
  std::atomic<int> val;
};

void eachThreadHasOwnAlignedAtomic(std::vector<AlignedAtomic>& alignedAtomics, int n) {
  std::vector<std::jthread> threads(n);
  for (int i = 0; i < n; ++i) {
    threads[i] = std::jthread([&]() { work(alignedAtomics[i].val, n); });
  }
}

static void BM_NoSharing(benchmark::State& state) {
  const int n = state.range(0);
  std::vector<AlignedAtomic> atomics(n);
  for (auto _ : state) {
    eachThreadHasOwnAlignedAtomic(atomics, n);
  }
}

BENCHMARK(BM_NoSharing)
    ->Name("BM_NoSharing_test_with_#_threads")
    ->RangeMultiplier(2)
    ->Range(1, 8)
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);

} // namespace cache_behavior