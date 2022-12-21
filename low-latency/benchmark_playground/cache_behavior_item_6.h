#include <benchmark/benchmark.h>

#include <array>
#include <thread>

namespace cache_behavior {

void work(std::vector<std::atomic<int>>& arr, int position) {
  for (int j = 0; j < 100000000; j++) {
    arr[position] = arr[position] + 3;
  }
}

void sameFullOpsCntDifferentPos(std::vector<std::atomic<int>>& arr,
                                int offset) {
  std::vector<std::jthread> threads(4);
  for (int i = 0; i < 4; ++i) {
    threads[i] = std::jthread([&]() { work(arr, i * offset); });
  }
}

static void BM_falseSharing_Example2(benchmark::State& state) {
  std::vector<std::atomic<int>> arr(64);
  for (auto _ : state) {
    sameFullOpsCntDifferentPos(arr, state.range(0));
  }
}

BENCHMARK(BM_falseSharing_Example2)
    ->Name("FalseSharing_4_thread_same_ops_different_offset")
    ->Arg(1)
    ->Arg(16)
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);

}  // namespace cache_behavior