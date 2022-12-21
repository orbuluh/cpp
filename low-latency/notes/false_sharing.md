# Cache consistency and false sharing

Benchmark playground [example 1](../benchmark_playground/falseSharing.h) and [example 2](../benchmark_playground/cache_behavior_item_6.h) about different ways to showcase false sharing.

Resource

- [CoffeeBeforeArch YT](https://youtu.be/FygXDrRsaU8)
- [CoffeeBeforeArch Blog post](https://coffeebeforearch.github.io/2019/12/28/false-sharing-tutorial.html)
- [Example 6 in Gallery of Processor Cache Effects](http://igoro.com/archive/gallery-of-processor-cache-effects/)

Other cache behavior check [this note](cache_effects.md)


**What**

On multi-core machines, caches encounter another problem – **consistency**.

- Different cores have fully or partly separate caches. On some machine, L1 caches are separate (as is common), and there are two pairs of processors, each pair sharing an L2 cache. While the details vary, a modern multi-core machine will have a multi-level cache hierarchy, where the faster and smaller caches belong to individual processors.

**When one processor modifies a value in its cache, other processors cannot use the old value anymore.**

- That memory location will be invalidated in all of the caches.
- Furthermore, since **caches operate on the granularity of cache lines and not individual bytes, the entire cache line will be invalidated in all caches!**


## Direct sharing

Multiple threads writing to same data structure can lead to performance hit because data might end up bouncing around different cores/processors.

- Say we have an atomic integer, when one processor want to do an increment, it has to go over where the cache block is, which maybe in another processor/core, then it has to invalidate that cache line, and bring it over to its processor. By doing so we maintain the exclusively write.
- If everyone tries to write data on the same cache block, the data will therefore need to bounce around different cores. So it's likely most of the time are spent on data transfer between cores instead of computation that we want.

## False sharing

Occurs when we unintentionally share data as data get transferred at the cache-line granularity.

- Symptom of data layout and architecture
- Unrelated data can get mapped to the same cache line.
- Check [example 1](../benchmark_playground/falseSharing.h) and [example 2](../benchmark_playground/cache_behavior_item_6.h)


## How to know L1 cache size?

[C++17 `std::hardware_destructive_interference_size`, `std::hardware_constructive_interference_size`](https://en.cppreference.com/w/cpp/thread/hardware_destructive_interference_size)

or run command line:

```bash
getconf LEVEL1_DCACHE_LINESIZE
```

## Running `perf` with your benchmark

```bash
# inside benchmark_playground
perf record build/playground --benchmark_filter=BM_directSharing
# above will create perf.data in pwd, then run
perf report
# this will show some details, lots of time is used for lock

# if you are not using VM nor WSL ... you could use::
perf stat -d -d -d build/playground
# and it will show "L1-dcache-load-misses:u" ...etc
# (VM doesn't support such counter)
# and you can use filter to compare.
# directSharing and falseSharing in YT have about 30-40% L1 cache missed
# noSharing will have much lower (~3%) or so of L1 cache missed.
```