#include <benchmark/benchmark.h>
#include <limits>
#include <array>

namespace cache_behavior {

static constexpr auto arrSz = 32 * 1024 * 1024;

void computePerStep(std::array<int, arrSz>& arr, int stepSz) {
  for (int i = 0; i < arrSz; i += stepSz) {
    arr[i] *= 3;
  }
}

static void BM_CacheLineEffect(benchmark::State& state) {
  std::array<int, arrSz> arr = {};
  for (auto _ : state) {
    computePerStep(arr, state.range(0));
  }
}

BENCHMARK(BM_CacheLineEffect)
    ->Name("BM_CacheLineEffect_Step")
    ->RangeMultiplier(2)
    ->Range(1, 1024)
    ->Unit(benchmark::kMillisecond);

} // namespace cache_behavior