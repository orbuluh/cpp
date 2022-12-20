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
BM_Baseline      0.134 ms        0.134 ms         5303
BM_Every16       0.141 ms        0.141 ms         4815
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

- (Although not directly in my exp result when step size 16) while step is in the range from 1 to 16, the running time of the for-loop hardly changes. But from 16 onwards, the running time is halved each time we double the step.
- The reason behind this is that today’s CPUs do not access memory byte by byte. Instead, **they fetch memory in chunks of (typically) 64 bytes, called cache lines**.
- When you read a particular memory location, the entire cache line is fetched from the main memory into the cache. And, **accessing other values from the same cache line is cheap!**
- Since 16 ints take up 64 bytes (one cache line), for-loops with a step between 1 and 16 have to touch the same number of cache lines: all of the cache lines in the array. But once the step is 32, we’ll only touch roughly every other cache line, and once it is 64, only every fourth.
- Understanding of cache lines can be important for certain types of program optimizations. For example, alignment of data may determine whether an operation touches one or two cache lines. As we saw in the example above, this can easily mean that in the misaligned case, the operation will be twice slower.
