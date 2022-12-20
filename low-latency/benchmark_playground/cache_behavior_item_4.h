#include <benchmark/benchmark.h>
#include <limits>
#include <array>

namespace cache_behavior {

static constexpr auto accessTime = 1024 * 1024 * 1024;

void computeOnSameElement(std::array<int, 2>& arr) {
  for (int i = 0; i < accessTime; i++) {
    benchmark::DoNotOptimize(arr[0]++);
    benchmark::DoNotOptimize(arr[0]++);
  }
}

static void BM_SameElement(benchmark::State& state) {
  std::array<int, 2> arr = {};
  for (auto _ : state) {
    computeOnSameElement(arr);
  }
}

BENCHMARK(BM_SameElement)
    ->Name("BM_SameElement")
    ->Unit(benchmark::kMillisecond);

void computeOnDiffElement(std::array<int, 2>& arr) {
  for (int i = 0; i < accessTime; i++) {
    benchmark::DoNotOptimize(arr[0]++);
    benchmark::DoNotOptimize(arr[1]++);
  }
}

static void BM_DiffElement(benchmark::State& state) {
  std::array<int, 2> arr = {};
  for (auto _ : state) {
    computeOnDiffElement(arr);
  }
}

BENCHMARK(BM_DiffElement)
    ->Name("BM_DiffElement")
    ->Unit(benchmark::kMillisecond);

} // namespace cache_behavior