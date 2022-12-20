# SIMD / Vectorization

## [What is "vectorization"?](https://stackoverflow.com/a/1422181/4924135)

- Many CPUs have "vector" or "SIMD" instruction sets which **apply the same operation simultaneously to two, four, or more pieces of data**. Modern x86 chips have the **SSE** instructions, many PPC chips have the "**Altivec**" instructions, and even some ARM chips have a vector instruction set, called **NEON**.
- "Vectorization" (simplified) is the process of **rewriting a loop so that instead of processing a single element of an array N times, it processes (say) 4 elements of the array simultaneously N/4 times.**
  - I chose 4 because it's what modern hardware is most likely to directly support for 32-bit floats or ints.
- The difference between vectorization and loop unrolling: Consider the following very simple loop that adds the elements of two arrays and stores the results to a third array.

```cpp
for (int i = 0; i < 16; ++i)
    C[i] = A[i] + B[i];
```

Unrolling this loop would transform it into something like this:

```cpp
for (int i = 0; i < 16; i += 4) {
    C[i]   = A[i]   + B[i];
    C[i+1] = A[i+1] + B[i+1];
    C[i+2] = A[i+2] + B[i+2];
    C[i+3] = A[i+3] + B[i+3];
}
```

- Vectorizing it, on the other hand, produces something like this:

```cpp
for (i = 0; i < 16; i+=4) {
   // conceptual notation to whatever intrinsic(s) your compiler uses to specify
   // vector instructions
   C[i:i+3] = A[i:i+3] * B[i:i+3];
}
```

**Terminology**

- Note that most modern ahead-of-time compilers are able to auto vectorize very simple loops like this, which can often be enabled via a compile option (on by default with full optimization in modern C and C++ compilers, like `gcc -O3 -march=native`).
- OpenMP `#pragma omp simd` is sometimes helpful to hint the compiler, especially for "reduction" loops like summing an FP array where vectorization requires pretending that FP math is associative.
- More complex algorithms still require help from the programmer to generate good vector code; we call this manual vectorization, often with intrinsics like
  - x86 `_mm_add_ps` that map to a single machine instruction as in SIMD prefix sum on Intel cpu
  - or How to count character occurrences using SIMD.
  - Or even use SIMD for short non-looping problems like Most insanely fastest way to convert 9 char digits into an int or unsigned int or
  - How to convert a binary integer number to a hex string?

## [Do we need vectorization in C++](https://stackoverflow.com/a/66100443/4924135)

C++ compilers support auto-vectorization, although if you need to have vectorization, then you might not be able to rely on such optimization because not every loop can be vectorized automatically.

Some things that you could do to potentially achieve vectorization in standard C++:

- Enable compiler-vectorizations that perform auto vectorization. (See the manual of your compiler)
- Specify a target CPU that has vector operations in their instruction set. (See the manual of your compiler)
- Use standard algorithms with `std::parallel_unsequenced_policy` or `std::unsequenced_policy`.
- Ensure that the data being operated on is sufficiently aligned for SIMD instructions. You can use `alignas`. See the manual of the target CPU for what alignment you need.
- Ensure that the optimizer knows as much as possible by using link time optimization.
- Partially unroll your loops. Limitation of this is that you hard code the amount of parallelization:

```cpp
for (int i = 0; i < count; i += 4) {
    operation(i + 0);
    operation(i + 1);
    operation(i + 2);
    operation(i + 3);
}
```

Outside of standard, portable C++, there are implementation specific ways:

- Some compilers provide language extension to write explicitly vectorized programs. This is portable across different CPUs but not portable to compilers that don't implement the extension.

```cpp
using v4si = int __attribute__ ((vector_size (16)));
v4si a, b, c;
a = b + 1;    /* a = b + {1,1,1,1}; */
a = 2 * b;    /* a = {2,2,2,2} * b; */

```
- Some compilers provide "builtin" functions to invoke specific CPU instructions which can be used to invoke SIMD vector instructions. Using these is **not portable across incompatible CPUs**.
- Some compilers support OpenMP API which has `#pragma omp` simd.

## [Guide C++ compiler to auto vectorized the code](https://betterprogramming.pub/guide-the-compiler-to-speed-up-your-code-655c1902b262)

- Modern CPUs have capabilities and support for SIMD which stands for **Single Instruction, Multiple Data**.
- Such machines can exhibit data-level parallelism (which is different from concurrency). They can perform the same instruction on multiple data at once.
- SIMD instruction for + is called addps in SSE or vaddps in AVX, and they support grouping of 4 elements and 8 elements respectively (integer type).
- SIMD supports instructions that can operate on vector data types. Vectorization is the use of vector instructions to speed up program execution.
- Vectorization can be done both by programmers — by explicitly writing vector instructions as well as by the compiler directly. The latter case is called Auto Vectorization.
- Auto Vectorization can be done by Ahead of Time (AOT) compilers at compile time or by Just in Time (JIT) compiler at execution time.
- Often our scalar codes are actually being run as vector instructions in the CPUs already! ...However, it depends on how the code is written ... - Sometimes, the compiler cannot determine if it’s safe to vectorized a certain loop or not. Same goes for loop unrolling. ... There are ways to guide your compiler that it’s safe to compile with these optimizations.

`#pragma clang loop vectorize(assume_safety)`

- Using following #pragma declaration just before the for loop indicates to the compiler that the following loop contains no data dependencies that should prevent auto-vectorization

`#pragma clang loop unroll_count(4)`

- Similarly, we can instruct the compiler to unroll loops when compiling with this pragma. The integer in unroll_count(N) basically guides the compiler on how much to unroll — you can benchmark with different numbers to figure out the best one.