#include <benchmark/benchmark.h>

#include <array>

namespace cache_behavior {

static constexpr auto accessTime = 64 * 1024 * 1024; // Arbitrary number

// Doing access on every 16 ints (to make sure each access is on its cache line)
// And then compare: for different size of array, with same number of access,
// what would be the access time? When the array size exceed L1/L2/L3 maximum
// there should be a gap!

template <int SIZE>
void computeEvery16() {
  int arr[SIZE] = {};
  const int lengthMod = SIZE - 1;
  for (int i = 0; i < accessTime; i++) {
    arr[(i * 16) & lengthMod]++; // (x & lengthMod) is equal to (x % arr.Length)
  }
}

template <int SIZE>
static void BM_L1L2L3Effect(benchmark::State& state) {
  for (auto _ : state) {
    computeEvery16<SIZE / 4>();
  }
}

BENCHMARK(BM_L1L2L3Effect<1 * 1024>)->Name("L1L2L3Effect_ArrSz_1K")->Unit(benchmark::kMillisecond);
BENCHMARK(BM_L1L2L3Effect<2 * 1024>)->Name("L1L2L3Effect_ArrSz_2k")->Unit(benchmark::kMillisecond);
BENCHMARK(BM_L1L2L3Effect<4 * 1024>)->Name("L1L2L3Effect_ArrSz_4k")->Unit(benchmark::kMillisecond);
BENCHMARK(BM_L1L2L3Effect<8 * 1024>)->Name("L1L2L3Effect_ArrSz_8k")->Unit(benchmark::kMillisecond);
BENCHMARK(BM_L1L2L3Effect<16 * 1024>)->Name("L1L2L3Effect_ArrSz_16k")->Unit(benchmark::kMillisecond);
BENCHMARK(BM_L1L2L3Effect<32 * 1024>)->Name("L1L2L3Effect_ArrSz_32k")->Unit(benchmark::kMillisecond);
BENCHMARK(BM_L1L2L3Effect<64 * 1024>)->Name("L1L2L3Effect_ArrSz_64k")->Unit(benchmark::kMillisecond);
BENCHMARK(BM_L1L2L3Effect<128 * 1024>)->Name("L1L2L3Effect_ArrSz_128k")->Unit(benchmark::kMillisecond);
BENCHMARK(BM_L1L2L3Effect<256 * 1024>)->Name("L1L2L3Effect_ArrSz_256k")->Unit(benchmark::kMillisecond);
BENCHMARK(BM_L1L2L3Effect<512 * 1024>)->Name("L1L2L3Effect_ArrSz_512k")->Unit(benchmark::kMillisecond);
BENCHMARK(BM_L1L2L3Effect<1024 * 1024>)->Name("L1L2L3Effect_ArrSz_1m")->Unit(benchmark::kMillisecond);
BENCHMARK(BM_L1L2L3Effect<16 * 1024 * 1024>)->Name("L1L2L3Effect_ArrSz_16m")->Unit(benchmark::kMillisecond);
BENCHMARK(BM_L1L2L3Effect<32 * 1024 * 1024>)->Name("L1L2L3Effect_ArrSz_32m")->Unit(benchmark::kMillisecond);
BENCHMARK(BM_L1L2L3Effect<64 * 1024 * 1024>)->Name("L1L2L3Effect_ArrSz_64m")->Unit(benchmark::kMillisecond);

} // namespace cache_behavior