#include <benchmark/benchmark.h>

#include <array>

namespace cache_behavior {

static constexpr int iterToRun = 200000000;

void run1() {
  int a = 0, b = 0, c = 0, d = 0, e = 0, f = 0, g = 0;
  for (int i = 0; i < iterToRun; ++i) {
    benchmark::DoNotOptimize(a++);
    benchmark::DoNotOptimize(b++);
    benchmark::DoNotOptimize(c++);
    benchmark::DoNotOptimize(d++);
  }
}

void run2() {
  int a = 0, b = 0, c = 0, d = 0, e = 0, f = 0, g = 0;
  for (int i = 0; i < iterToRun; ++i) {
    benchmark::DoNotOptimize(a++);
    benchmark::DoNotOptimize(c++);
    benchmark::DoNotOptimize(e++);
    benchmark::DoNotOptimize(g++);
  }
}

void run3() {
  int a = 0, b = 0, c = 0, d = 0, e = 0, f = 0, g = 0;
  for (int i = 0; i < iterToRun; ++i) {
    benchmark::DoNotOptimize(a++);
    benchmark::DoNotOptimize(c++);
  }
}

template <int item>
static void BM_Run(benchmark::State& state) {
  for (auto _ : state) {
    if constexpr (item == 1) {
      run1();
    } else if constexpr (item == 2) {
      run2();
    } else {
      run3();
    }
  }
}

BENCHMARK(BM_Run<1>);//->Unit(benchmark::kMillisecond);
BENCHMARK(BM_Run<2>);//->Unit(benchmark::kMillisecond);
BENCHMARK(BM_Run<3>);//->Unit(benchmark::kMillisecond);

}  // namespace cache_behavior