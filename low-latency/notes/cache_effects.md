# Cache effects

> Taking notes from [Gallery of Processor Cache Effects from Igor Ostrovsky Blogging](http://igoro.com/archive/gallery-of-processor-cache-effects/)


## Item 1. Memory accesses is much slower than computation

[Code](../benchmark_playground/cache_behavior_item_1.h)

<details><summary markdown="span">Exp result</summary>


```cpp
void computeBaseline(std::array<int, arrSz>& arr) {
  for (int i = 0; i < arrSz; ++i) {
    arr[i] *= 3;
  }
}

//v.s.

void computeEvery16(std::array<int, arrSz>& arr) {
  for (int i = 0; i < arrSz; i += 16) {
    arr[i] *= 3;
  }
}
```
...

```bash
------------------------------------------------------
Benchmark            Time             CPU   Iterations
------------------------------------------------------
BM_Baseline       10.0 ms         10.0 ms           69
BM_Every16        9.71 ms         9.71 ms           73
```

</details>

- The first loop multiplies every value in the array by 3, and the second loop multiplies only every 16-th.
- The second loop only does about 6% of the work of the first loop, but on modern machines, the two for-loops take about the same time.
- The reason why the loops take the same amount of time has to do with memory. **The running time of these loops is dominated by the memory accesses to the array, not by the integer multiplications.** And, **the hardware will perform the same main memory accesses for the two loops**. (Check item 2)

## Item 2: Impact of cache lines

[Code](../benchmark_playground/cache_behavior_item_2.h)

<details><summary markdown="span">Exp result</summary>


```cpp
void computeEveryStep(std::array<int, arrSz>& arr, int stepSz) {
  for (int i = 0; i < arrSz; i += stepSz) {
    arr[i] *= 3;
  }
}
```
...

```bash
-----------------------------------------------------------------------
Benchmark                             Time             CPU   Iterations
-----------------------------------------------------------------------
BM_CacheLineEffect_Step/1         0.252 ms        0.252 ms         2787
BM_CacheLineEffect_Step/2         0.133 ms        0.133 ms         5322
BM_CacheLineEffect_Step/4         0.133 ms        0.133 ms         5221
BM_CacheLineEffect_Step/8         0.121 ms        0.121 ms         5728
BM_CacheLineEffect_Step/16        0.125 ms        0.125 ms         5323
BM_CacheLineEffect_Step/32        0.115 ms        0.115 ms         6185
BM_CacheLineEffect_Step/64        0.080 ms        0.080 ms         8745
BM_CacheLineEffect_Step/128       0.043 ms        0.043 ms        16317
BM_CacheLineEffect_Step/256       0.016 ms        0.016 ms        43357
BM_CacheLineEffect_Step/512       0.010 ms        0.010 ms        75267
BM_CacheLineEffect_Step/1024      0.005 ms        0.005 ms       151489
```

</details>

- (Although not directly in my exp result when step size 16) while step is in the range from 1 to 16, the running time of the for-loop hardly changes. But from 16 onwards, the running time is halved each time we double the step.
- The reason behind this is that today’s CPUs do not access memory byte by byte. Instead, **they fetch memory in chunks of (typically) 64 bytes, called cache lines**.
- When you read a particular memory location, the entire cache line is fetched from the main memory into the cache. And, **accessing other values from the same cache line is cheap!**
- Since 16 ints take up 64 bytes (one cache line), for-loops with a step between 1 and 16 have to touch the same number of cache lines: all of the cache lines in the array. But once the step is 32, we’ll only touch roughly every other cache line, and once it is 64, only every fourth.
- Understanding of cache lines can be important for certain types of program optimizations. For example, alignment of data may determine whether an operation touches one or two cache lines. As we saw in the example above, this can easily mean that in the misaligned case, the operation will be twice slower.

## Item 3: L1 / L2 / L3 cache sizes

[Code](../benchmark_playground/cache_behavior_item_3.h)

<details><summary markdown="span">Exp result</summary>


```cpp
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
```
...

```bash
------------------------------------------------------------------
Benchmark                        Time             CPU   Iterations
------------------------------------------------------------------
L1L2L3Effect_ArrSz_1K         14.8 ms         14.8 ms           48
L1L2L3Effect_ArrSz_2k         14.3 ms         14.3 ms           48
L1L2L3Effect_ArrSz_4k         14.4 ms         14.4 ms           49
L1L2L3Effect_ArrSz_8k         14.3 ms         14.3 ms           48
L1L2L3Effect_ArrSz_16k        14.4 ms         14.4 ms           48
L1L2L3Effect_ArrSz_32k        14.3 ms         14.3 ms           48
L1L2L3Effect_ArrSz_64k        54.4 ms         54.4 ms           13
L1L2L3Effect_ArrSz_128k       54.9 ms         54.9 ms           13
L1L2L3Effect_ArrSz_256k       54.5 ms         54.5 ms           13
L1L2L3Effect_ArrSz_512k       54.6 ms         54.6 ms           12
L1L2L3Effect_ArrSz_1m         68.1 ms         68.1 ms           11
L1L2L3Effect_ArrSz_16m         145 ms          145 ms            5
L1L2L3Effect_ArrSz_32m         177 ms          177 ms            4
L1L2L3Effect_ArrSz_64m         272 ms          272 ms            2
```

</details>

If you want to know the sizes of the different caches, you can use the [CoreInfo SysInternals tool](https://learn.microsoft.com/en-us/sysinternals/downloads/coreinfo)
- Note: The L1 caches are per-core, and the L2 caches are shared between pairs of cores:

```bash
Logical Processor to Cache Map:
**------------------  Data Cache          0, Level 1,   48 KB, Assoc  12, LineSize  64
**------------------  Instruction Cache   0, Level 1,   32 KB, Assoc   8, LineSize  64
**------------------  Unified Cache       0, Level 2,    1 MB, Assoc  10, LineSize  64
********************  Unified Cache       1, Level 3,   25 MB, Assoc  10, LineSize  64
...
```

So my computer has 48K K1, 1MB L2, 25MB L3, which is reflected in the exp result.


## Item 4: Instruction-level parallelism

[Code](../benchmark_playground/cache_behavior_item_4.h)

<details><summary markdown="span">Exp result</summary>


```cpp
void computeOnSameElement(std::array<int, 2>& arr) {
  for (int i = 0; i < accessTime; i++) {
    benchmark::DoNotOptimize(arr[0]++);
    benchmark::DoNotOptimize(arr[0]++);
  }
}

// v.s.

void computeOnDiffElement(std::array<int, 2>& arr) {
  for (int i = 0; i < accessTime; i++) {
    benchmark::DoNotOptimize(arr[0]++);
    benchmark::DoNotOptimize(arr[1]++);
  }
}
```
...

```bash
---------------------------------------------------------
Benchmark               Time             CPU   Iterations
---------------------------------------------------------
BM_SameElement        369 ms          369 ms            2
BM_DiffElement        254 ms          254 ms            3
```

</details>

- It turns out that the second loop is faster than the first loop. (Not 2x in my exp. And it's all 0 when optimized.)
- Why? This has to do with the dependencies between operations in the two loop bodies.
- In the body of the first loop, operations depend on each other as follows:
  - `x = a[0]` -> `x++` -> `a[0] = x` -> `y = a[0]` -> `y++` -> `a[0] = y`
- But in the second loop, we only have:
  - `x = a[0]` -> `x++` -> `a[0] = x`
  - `y = a[1]` -> `y++` -> `a[1] = y`
- The modern processor has various parts that have a little bit of parallelism in them: it can access two memory locations in L1 at the same time, or perform two simple arithmetic operations. In the first loop, the processor cannot exploit this instruction-level parallelism, but in the second loop, it can.

More reads on [SIMD/vectorization](simd_vectorization.md)

