# Cache effects

> Taking notes from [Gallery of Processor Cache Effects from Igor Ostrovsky Blogging](http://igoro.com/archive/gallery-of-processor-cache-effects/)


## Item 1. Memory accesses is much slower than computation

[Code](../benchmark_playground/memAccessAndCompute.h)

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