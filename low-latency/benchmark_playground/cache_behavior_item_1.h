#include <benchmark/benchmark.h>

#include <array>

namespace cache_behavior {

static constexpr auto arrSz = 32 * 1024 * 1024;

void computeBaseline(std::array<int, arrSz>& arr) {
  for (int i = 0; i < arrSz; ++i) {
    arr[i] *= 3;
  }
}

static void BM_Baseline(benchmark::State& state) {
  std::array<int, arrSz> arr = {};
  for (auto _ : state) {
    computeBaseline(arr);
  }
}

BENCHMARK(BM_Baseline)->Unit(benchmark::kMillisecond);

//------------------------------------------------------------

void computeEvery16(std::array<int, arrSz>& arr) {
  for (int i = 0; i < arrSz; i += 16) {
    arr[i] *= 3;
  }
}

static void BM_Every16(benchmark::State& state) {
  std::array<int, arrSz> arr = {};
  for (auto _ : state) {
    computeEvery16(arr);
  }
}

BENCHMARK(BM_Every16)->Unit(benchmark::kMillisecond);

} // namespace cache_behavior