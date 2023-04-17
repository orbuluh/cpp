// from Concurrency in C++: A Programmer’s Overview (part 2 of 2) - Fedor Pikus

#include <benchmark/benchmark.h>

#include <algorithm>
#include <execution>
#include <unistd.h>
#include <math.h>
#include <vector>

namespace std_parallel {

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

auto calc = [](double& x) { x = sin(x) + cos(x) * exp(-x); };


static void BM_ForEach(benchmark::State& state) {
  const size_t N = state.range(0);
  std::vector<double> v(N);
  std::for_each(v.begin(), v.end(), [](double& x) { x = rand(); });
  for (auto _ : state) {
    std::for_each(v.begin(), v.end(), calc);
  }
  state.SetItemsProcessed(N * state.iterations());
}

static void BM_ForEach_Seq(benchmark::State& state) {
  const size_t N = state.range(0);
  std::vector<double> v(N);
  std::for_each(v.begin(), v.end(), [](double& x) { x = rand(); });
  for (auto _ : state) {
    std::for_each(std::execution::seq, v.begin(), v.end(), calc);
  }
  state.SetItemsProcessed(N * state.iterations());
}

static void BM_ForEach_Par(benchmark::State& state) {
  const size_t N = state.range(0);
  std::vector<double> v(N);
  std::for_each(v.begin(), v.end(), [](double& x) { x = rand(); });
  for (auto _ : state) {
    std::for_each(std::execution::par, v.begin(), v.end(), calc);
  }
  state.SetItemsProcessed(N * state.iterations());
}

static void BM_ForEach_Unseq(benchmark::State& state) {
  const size_t N = state.range(0);
  std::vector<double> v(N);
  std::for_each(v.begin(), v.end(), [](double& x) { x = rand(); });
  for (auto _ : state) {
    std::for_each(std::execution::unseq, v.begin(), v.end(), calc);
  }
  state.SetItemsProcessed(N * state.iterations());
}

#define ARGS \
  ->RangeMultiplier(32)\
  ->Range(1024, 1024 * 1024)\
  ->UseRealTime()\
  ->Unit(benchmark::kMillisecond)

BENCHMARK(BM_ForEach)->Name("BM_ForEach_#_items") ARGS;
BENCHMARK(BM_ForEach_Seq)->Name("BM_ForEach_Seq_#_items") ARGS;
BENCHMARK(BM_ForEach_Par)->Name("BM_ForEach_Par_#_items") ARGS;
BENCHMARK(BM_ForEach_Unseq)->Name("BM_ForEach_Unseq_#_items") ARGS;

}  // namespace std_parallel