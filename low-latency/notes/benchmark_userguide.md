# [Benchmark user guide](https://github.com/google/benchmark/blob/main/docs/user_guide.md)

Also check [benchmark docsforge](https://benchmark.docsforge.com/)

---

## How many iteration?

When the benchmark binary is executed, **each benchmark function is run serially.** The number of iterations to run is **determined dynamically**.

- by running the benchmark a few times and measuring the time taken and ensuring that the ultimate result will be statistically stable.
- As such, **faster benchmark functions will be run for more iterations than slower benchmark functions**, and the number of iterations is thus reported.
- In all cases, the number of iterations for which the benchmark is run is governed by the amount of time the benchmark takes.
- Concretely, the number of iterations is **at least one, not more than 1e9, until CPU time is greater than the minimum time, or the wallclock time is 5x minimum time.**
- The minimum time is set per benchmark by calling `MinTime` on the registered benchmark object.

---

## Warmup

Furthermore warming up a benchmark might be necessary in order to get stable results **because of e.g caching effects of the code under benchmark**.

- Warming up means **running the benchmark a given amount of time, before results are actually taken into account.**
- The amount of time for which the warmup should be run can be set per benchmark by calling `MinWarmUpTime` on the registered benchmark object or for all benchmarks using the `--benchmark_min_warmup_time` command-line option.
- Note that `MinWarmUpTime` will overwrite the value of `--benchmark_min_warmup_time` for the single benchmark.
- How many iterations the warmup run of each benchmark takes is determined the same way as described in the paragraph above. Per **default the warmup phase is set to 0 seconds and is therefore disabled.**

---

## Report

Average timings are then reported over the **iterations run**.

- If multiple repetitions are requested using the `--benchmark_repetitions` command-line option, or at registration time, the benchmark function will be run several times and statistical results across these repetitions will also be reported. As well as the per-benchmark entries, a preamble in the report will include information about the machine on which the benchmarks are run.

---

## Setup/Teardown

Global setup/teardown specific to each benchmark can be done by passing a callback to `Setup`/`Teardown`:

- The setup/teardown callbacks will be invoked once for each benchmark.
- If the benchmark is multi-threaded (will run in k threads), they will be invoked exactly once before each run with k threads.
- If the benchmark uses different size groups of threads, the above will be true for each size group.

```cpp
static void DoSetup(const benchmark::State& state) {}

static void DoTeardown(const benchmark::State& state) {}

static void BM_func(benchmark::State& state){...}

BENCHMARK(BM_func)
    ->Arg(1)
    ->Arg(3)
    ->Threads(16)
    ->Threads(32)
    ->Setup(DoSetup)
    ->Teardown(DoTeardown);

// DoSetup and DoTearDown will be invoked 4 times each
// specifically, once for each of this family:
// BM_func_Arg_1_Threads_16
// BM_func_Arg_1_Threads_32
// BM_func_Arg_3_Threads_16
// BM_func_Arg_3_Threads_32
```

---

## Passing arguments

**1 argument**

```cpp
void func(int arg) { /*do something*/
}

static void BM_fnc(benchmark::State& state) {
  for (auto _ : state) {
    func(state.range(0));
  }
}

// run: [ 8, 64, 512, 4k, 8k ]
BENCHMARK(BM_fnc)->Range(8, 8 << 10);

// run: [ 8, 16, 32, 64, 128, 256, 512, 1024, 2k, 4k, 8k ]
BENCHMARK(BM_fnc)->RangeMultiplier(2)->Range(8, 8 << 10);

// run: [ 0, 128, 256, 384, 512, 640, 768, 896, 1024 ]
BENCHMARK(BM_fnc)->DenseRange(0, 1024, 128);
```

**2 arguments**

```cpp
void func(int arg1, int arg2) { /*do something*/
}

static void BM_fnc(benchmark::State& state) {
  for (auto _ : state) {
    func(state.range(0), state.range(1));
  }
}

BENCHMARK(BM_fnc)
    ->Args({1 << 10, 128})
    ->Args({2 << 10, 128})
    ->Args({4 << 10, 128})
    ->Args({8 << 10, 128})
    ->Args({1 << 10, 512})
    ->Args({2 << 10, 512})
    ->Args({4 << 10, 512})
    ->Args({8 << 10, 512});

// equivalent to
BENCHMARK(BM_fnc)->Ranges({{1 << 10, 8 << 10}, {128, 512}});
```

**2 arguments that can't be represented as range**

```cpp
BENCHMARK(BM_fnc)
    ->ArgsProduct({{1 << 10, 3 << 10, 8 << 10}, {20, 40}})

// would generate the same benchmark arguments as
BENCHMARK(BM_fnc)
    ->Args({1 << 10, 20})
    ->Args({3 << 10, 20})
    ->Args({8 << 10, 20})
    ->Args({1 << 10, 40})
    ->Args({3 << 10, 40})
    ->Args({8 << 10, 40});
```

**Can also use macro to generate subrange**

```cpp
BENCHMARK(BM_fnc)
    ->ArgsProduct({benchmark::CreateRange(8, 128, /*multi=*/2),
                   benchmark::CreateDenseRange(1, 4, /*step=*/1)})
// would generate the same benchmark arguments as
BENCHMARK(BM_fnc)
    ->ArgsProduct({{8, 16, 32, 64, 128}, {1, 2, 3, 4}});
```

**an arbitrary set of arguments on which to run the benchmark**

```cpp
static void CustomArguments(benchmark::internal::Benchmark* b) {
  for (int i = 0; i <= 10; ++i) {
    for (int j = 32; j <= 1024 * 1024; j *= 8) {
      b->Args({i, j});
    }
  }
}
BENCHMARK(BM_SetInsert)->Apply(CustomArguments);
```

**arbitrary arguments**

```cpp
template <class... Args>
void BM_takes_args(benchmark::State& state, Args&&... args) {
  auto args_tuple = std::make_tuple(std::move(args)...);
  for (auto _ : state) {
    std::cout << std::get<0>(args_tuple) << ": " << std::get<1>(args_tuple)
              << '\n';
    //[...]
  }
}
// Registers a benchmark named "BM_takes_args/int_string_test" that passes
// the specified values to `args`.
BENCHMARK_CAPTURE(BM_takes_args, int_string_test, 42, std::string("abc"));

// Registers the same benchmark "BM_takes_args/int_test" that passes
// the specified values to `args`.
BENCHMARK_CAPTURE(BM_takes_args, int_test, 42, 43);
```

> :rotating_light: Note that elements of `...args` may refer to global variables. Users should avoid modifying global state inside of a benchmark.

---

## Complexity report

- [List of Enum BigO](https://benchmark.docsforge.com/master/api/benchmark/BigO/)

```cpp
static void BM_StringCompare(benchmark::State& state) {
  std::string s1(state.range(0), '-');
  std::string s2(state.range(0), '-');
  for (auto _ : state) {
    benchmark::DoNotOptimize(s1.compare(s2));
  }
  state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_StringCompare)
    ->RangeMultiplier(2)->Range(1<<10, 1<<18)->Complexity(benchmark::oN);
```

[Represent multi variable complexity](https://stackoverflow.com/a/64679707/4924135)

- If you set the complexity to `M+N` and use `oN` then the fitting curve used for the minimal least square calculation will be linear in `M+N`.
- However, if you set the complexity to `M*N` and use `oNSquared` then we'll try to fit to `pow(M*N, 2)`

**Specify asymptotic complexity with a lambda function**

```cpp
BENCHMARK(BM_StringCompare)
    ->RangeMultiplier(2)
    ->Range(1 << 10, 1 << 18)
    ->Complexity([](benchmark::IterationCount n) -> double { return n; });
```

---

## Custom Benchmark Name

```cpp
BENCHMARK(BM_memcpy)->Name("memcpy")->RangeMultiplier(2)->Range(8, 8 << 10);
```

---

## Templated benchmarks

```cpp
template <typename Q>
void BM_Sequential(benchmark::State& state) {
  Q q;
  typename Q::value_type v;
  for (auto _ : state) {
    for (int i = state.range(0); i--;) q.push(v);
    for (int e = state.range(0); e--;) q.Wait(&v);
  }
  // actually messages, not bytes:
  state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                          state.range(0));
}

// C++11 or newer, you can use the BENCHMARK macro with template parameters:
BENCHMARK(BM_Sequential<WaitQueue<int>>)->Range(1 << 0, 1 << 10);
```

---

## Multithreaded Benchmarks

- In a multithreaded test (**benchmark invoked by multiple threads simultaneously**), it is guaranteed that **none of the threads will start until all have reached the start of the benchmark loop**, and all** will have finished before any thread exits the benchmark loop.** (This behavior is also provided by the `KeepRunning()` API)
- As such, any global setup or teardown can be wrapped in a check against the thread index:

```cpp
static void BM_MultiThreaded(benchmark::State& state) {
  if (state.thread_index() == 0) {
    // Setup code here.
  }
  for (auto _ : state) {
    // Run the test as normal.
  }
  if (state.thread_index() == 0) {
    // Teardown code here.
  }
}
BENCHMARK(BM_MultiThreaded)->Threads(2);
```

To run the benchmark across a range of thread counts, instead of Threads, use `ThreadRange`.

- This takes two parameters (`min_threads` and `max_threads`) and runs the benchmark once for values in the **inclusive range**. For example:

```cpp
BENCHMARK(BM_MultiThreaded)->ThreadRange(1, 8);
```

- will run BM_MultiThreaded with thread counts 1, 2, 4, and 8.

If the benchmarked code itself uses threads and **you want to compare it to single-threaded code**, you may want to use real-time ("wallclock") measurements for latency comparisons. Without `UseRealTime`, CPU time is used by default.

```cpp
BENCHMARK(BM_test)->Range(8, 8<<10)->UseRealTime();
```

---

## CPU Timers

- By default, the CPU timer **only measures the time spent by the main thread**.
- If the benchmark itself uses threads internally, this measurement may not be what you are looking for. Instead, there is a way to measure the total CPU usage of the process, by all the threads.

```cpp
void callee(int i);

static void MyMain(int size) {
  #pragma omp parallel for
  for (int i = 0; i < size; i++) {
    callee(i);
  }
}

static void BM_OpenMP(benchmark::State& state) {
  for (auto _ : state) {
    MyMain(state.range(0));
  }
}

// Measure the time spent by the main thread, use it to decide for how long to
// run the benchmark loop. Depending on the internal implementation detail may
// measure to anywhere from near-zero (the overhead spent before/after work
// handoff to worker thread[s]) to the whole single-thread time.
BENCHMARK(BM_OpenMP)->Range(8, 8 << 10);

// Measure the user-visible time, the wall clock (literally, the time that
// has passed on the clock on the wall), use it to decide for how long to
// run the benchmark loop. This will always be meaningful, an will match the
// time spent by the main thread in single-threaded case, in general decreasing
// with the number of internal threads doing the work.
BENCHMARK(BM_OpenMP)->Range(8, 8 << 10)->UseRealTime();

// Measure the total CPU consumption, use it to decide for how long to
// run the benchmark loop. This will always measure to no less than the
// time spent by the main thread in single-threaded case.
BENCHMARK(BM_OpenMP)->Range(8, 8 << 10)->MeasureProcessCPUTime();

// A mixture of the last two. Measure the total CPU consumption, but use the
// wall clock to decide for how long to run the benchmark loop.
BENCHMARK(BM_OpenMP)->Range(8, 8 << 10)->MeasureProcessCPUTime()->UseRealTime();
```

**Controlling Timers**

Normally, **the entire duration of the work loop `for (auto _ : state) {}` is measured.**

- But sometimes, it is necessary to do some work inside of that loop, every iteration, but without counting that time to the benchmark time.
- That is possible, **although it is not recommended, since it has high overhead.**

```cpp
static void BM_SetInsert_With_Timer_Control(benchmark::State& state) {
  std::set<int> data;
  for (auto _ : state) {
    // Stop timers. They will not count until they are resumed.
    state.PauseTiming();

    // Do something that should not be measured
    data = ConstructRandomSet(state.range(0));

    // And resume timers. They are now counting again. The rest will be measured.
    state.ResumeTiming();
    for (int j = 0; j < state.range(1); ++j) data.insert(RandomNumber());
  }
}
BENCHMARK(BM_SetInsert_With_Timer_Control)
    ->Ranges({{1 << 10, 8 << 10}, {128, 512}});
```

**Manual Timing**

For benchmarking something for which neither CPU time nor real-time are correct or accurate enough, completely manual timing is supported using the `UseManualTime` function.

- When `UseManualTime` is used, the benchmarked code **must call `SetIterationTime` once per iteration of the benchmark loop** to report the manually measured time.
- An example use case for this is benchmarking GPU execution (e.g. OpenCL or CUDA kernels, OpenGL or Vulkan or Direct3D draw calls), which cannot be accurately measured using CPU time or real-time. Instead, they can be measured accurately using a dedicated API, and these measurement results can be reported back with SetIterationTime.

```cpp
static void BM_ManualTiming(benchmark::State& state) {
  int microseconds = state.range(0);
  std::chrono::duration<double, std::micro> sleep_duration{
      static_cast<double>(microseconds)};

  for (auto _ : state) {
    auto start = std::chrono::high_resolution_clock::now();

    // Simulate some useful workload with a sleep
    std::this_thread::sleep_for(sleep_duration);
    auto end = std::chrono::high_resolution_clock::now();

    auto elapsed_seconds =
        std::chrono::duration_cast<std::chrono::duration<double>>(end - start);

    state.SetIterationTime(elapsed_seconds.count());
  }
}
BENCHMARK(BM_ManualTiming)->Range(1, 1 << 17)->UseManualTime();
```

---

## Setting the Time Unit

- If a benchmark runs a few milliseconds it may be hard to visually compare the measured times, since the output data is given in **nanoseconds per default**.
- In order to manually set the time unit, you can specify it manually:

```cpp
BENCHMARK(BM_test)->Unit(benchmark::kMillisecond);
```

- Additionally the default time unit can be set globally with the `--benchmark_time_unit={ns|us|ms|s}` command line argument. The argument **only affects benchmarks where the time unit is not set explicitly**.

---

## Memory Usage

It's often useful to also track memory usage for benchmarks, alongside CPU performance.

- For this reason, benchmark offers the `RegisterMemoryManager` method that allows a custom `MemoryManager` to be injected.
- If set, the `MemoryManager::Start` and `MemoryManager::Stop` methods will be called at the start and end of benchmark runs to allow user code to fill out a report on the number of allocations, bytes used, etc.
- This data will then be reported alongside other performance data, currently only when using JSON output.

---

## Fixtures

- 1. Defining a type that derives from `::benchmark::Fixture`
- 2. Creating/registering the tests using the following macros:
  - `BENCHMARK_F(ClassName, Method)`
  - `BENCHMARK_DEFINE_F(ClassName, Method)`
  - `BENCHMARK_REGISTER_F(ClassName, Method)`

```cpp
class MyFixture : public benchmark::Fixture {
 public:
  void SetUp(const ::benchmark::State& state) {}

  void TearDown(const ::benchmark::State& state) {}
};

BENCHMARK_F(MyFixture, FooTest)(benchmark::State& st) {
  for (auto _ : st) {
    ...
  }
}

BENCHMARK_DEFINE_F(MyFixture, BarTest)(benchmark::State& st) {
  for (auto _ : st) {
    ...
  }
}
/* BarTest is NOT registered */
BENCHMARK_REGISTER_F(MyFixture, BarTest)->Threads(2);
/* BarTest is now registered */
```

**Templated Fixtures**

- `BENCHMARK_TEMPLATE_F(ClassName, Method, ...)`
- `BENCHMARK_TEMPLATE_DEFINE_F(ClassName, Method, ...)`

```cpp
template <typename T>
class MyFixture : public benchmark::Fixture {};

BENCHMARK_TEMPLATE_F(MyFixture, IntTest, int)(benchmark::State& st) {
  for (auto _ : st) {
    ...
  }
}

BENCHMARK_TEMPLATE_DEFINE_F(MyFixture, DoubleTest, double)
(benchmark::State& st) {
  for (auto _ : st) {
    ...
  }
}

BENCHMARK_REGISTER_F(MyFixture, DoubleTest)->Threads(2);
```

---

## Preventing Optimization

To prevent a value or expression from being optimized away by the compiler the `benchmark::DoNotOptimize(...)` and `benchmark::ClobberMemory()` functions can be used.

```cpp
static void BM_test(benchmark::State& state) {
  for (auto _ : state) {
      int x = 0;
      for (int i=0; i < 64; ++i) {
        benchmark::DoNotOptimize(x += i);
      }
  }
}
```

- `DoNotOptimize(<expr>)` forces the result of `<expr>` to be stored in either memory or a register. For GNU based compilers it acts as read/write barrier for global memory. More specifically **it forces the compiler to flush pending writes to memory and reload any other values as necessary.**

- Note that `DoNotOptimize(<expr>)` **does not prevent optimizations on `<expr>` in any way. `<expr>` may even be removed entirely when the result is already known.** For example:


```cpp
  /* Example 1: `<expr>` is removed entirely. */
  int foo(int x) { return x + 42; }
  while (...) DoNotOptimize(foo(0)); // Optimized to DoNotOptimize(42);

  /*  Example 2: Result of '<expr>' is only reused */
  int bar(int) __attribute__((const));
  while (...) DoNotOptimize(bar(0)); // Optimized to:
  // int __result__ = bar(0);
  // while (...) DoNotOptimize(__result__);
```

The second tool for preventing optimizations is `ClobberMemory()`.

- In essence `ClobberMemory()` forces the compiler to **perform all pending writes to global memory.** - Memory managed by block scope objects must be "escaped" using `DoNotOptimize(...)` before it can be clobbered. In the below example `ClobberMemory()` prevents the call to `v.push_back(42)` from being optimized away.

```cpp
static void BM_vector_push_back(benchmark::State& state) {
  for (auto _ : state) {
    std::vector<int> v;
    v.reserve(1);
    benchmark::DoNotOptimize(v.data()); // Allow v.data() to be clobbered.
    v.push_back(42);
    benchmark::ClobberMemory(); // Force 42 to be written to memory.
  }
}
```

- Note that `ClobberMemory()` is only available for GNU or MSVC based compilers.


---

## Custom Counters

```cpp
static void UserCountersExample1(benchmark::State& state) {
  double numFoos = 0, numBars = 0, numBazs = 0;
  for (auto _ : state) {
    // ... count Foo,Bar,Baz events
  }
  state.counters.insert({{"Foo", numFoos}, {"Bar", numBars}, {"Baz", numBazs}});
  /* equivalent to:
  state.counters["Foo"] = numFoos;
  state.counters["Bar"] = numBars;
  state.counters["Baz"] = numBazs; */
}
```

- The `state.counters` object is a `std::map` with `std::string` keys and `Counter` values.
- `Counter` is a double-like class, you can use all of the standard arithmetic assignment operators (=,+=,-=,*=,/=) to change the value of each counter.
- In multithreaded benchmarks, each counter is set on the calling thread only. When the benchmark finishes, the counters from each thread will be summed; the resulting sum is the value which will be shown for the benchmark.


**The Counter constructor accepts 3 parameters**

1. the value as a double
2. a bit flag which allows you to show counters
   as rates,
   and/or as per-thread iteration,
   and/or as per-thread averages,
   and/or iteration invariants,
   and/or finally inverting the result;
3. and a flag specifying the 'unit'
   - i.e. is 1k a 1000 (default, `benchmark::Counter::OneK::kIs1000`), or 1024 (`benchmark::Counter::OneK::kIs1024`)?


```cpp
// sets a simple counter
state.counters["Foo"] = numFoos;

// Set the counter as a rate. It will be presented divided
// by the duration of the benchmark.
// Meaning: per one second, how many 'foo's are processed?
state.counters["FooRate"] = Counter(numFoos, benchmark::Counter::kIsRate);

// Set the counter as a rate. It will be presented divided
// by the duration of the benchmark, and the result inverted.
// Meaning: how many seconds it takes to process one 'foo'?
state.counters["FooInvRate"] =
    Counter(numFoos, benchmark::Counter::kIsRate | benchmark::Counter::kInvert);

// Set the counter as a thread-average quantity. It will
// be presented divided by the number of threads.
state.counters["FooAvg"] = Counter(numFoos, benchmark::Counter::kAvgThreads);

// There's also a combined flag:
state.counters["FooAvgRate"] =
    Counter(numFoos, benchmark::Counter::kAvgThreadsRate);

// This says that we process with the rate of state.range(0) bytes every
// iteration:
state.counters["BytesProcessed"] =
    Counter(state.range(0), benchmark::Counter::kIsIterationInvariantRate,
            benchmark::Counter::OneK::kIs1024);
```

You can use `--benchmark_counters_tabular=true` for a better custom counter reporting printout

---

## Statistics

By default **each benchmark is run once** and that single result is reported.

- However benchmarks are often noisy and a single result may not be representative of the overall behavior. For this reason it's possible to repeatedly rerun the benchmark.
- The number of runs of each benchmark is specified **globally** by the `--benchmark_repetitions` flag or on a **per benchmark basis** by calling `Repetitions` on the registered benchmark object.
- When a benchmark is run more than once the **mean, median, standard deviation and coefficient of variation of the runs will be reported.**

Additionally the `--benchmark_report_aggregates_only={true|false}`, `--benchmark_display_aggregates_only={true|false}` flags or `ReportAggregatesOnly(bool)`, `DisplayAggregatesOnly(bool)` functions can be used to change how repeated tests are reported.

- By default the result of each repeated run is reported.
- When report aggregates only option is true, only the aggregates (i.e. mean, median, standard deviation and coefficient of variation, maybe complexity measurements if they were requested) of the runs is reported, to **both the reporters - standard output (console), and the file**.
- However when only the display aggregates only option is true, only the aggregates are displayed in the **standard output**, while the **file output still contains everything**.
- Calling `ReportAggregatesOnly(bool)` / `DisplayAggregatesOnly(bool)` on a registered benchmark object overrides the value of the appropriate flag for that benchmark.

**Custom Statistics**

The following code will specify a custom statistic to be calculated, defined by a lambda function.

```cpp
void BM_spin_empty(benchmark::State& state) {
  for (auto _ : state) {
    for (int x = 0; x < state.range(0); ++x) {
      benchmark::DoNotOptimize(x);
    }
  }
}

BENCHMARK(BM_spin_empty)
    ->ComputeStatistics("max",
                        [](const std::vector<double>& v) -> double {
                          return *(
                              std::max_element(std::begin(v), std::end(v)));
                        })
    ->Arg(512);


// While usually the statistics produce values in time units, you can also produce percentages:

BENCHMARK(BM_spin_empty)
    ->ComputeStatistics(
        "ratio",
        [](const std::vector<double>& v) -> double {
          return std::begin(v) / std::end(v);
        },
        benchmark::StatisticUnit::kPercentage)
    ->Arg(512);
```

---

## Running Benchmarks

Benchmarks are executed by running the produced binaries.

- Benchmarks binaries, by default, accept options that may be specified either through their command line interface or by setting environment variables before execution.
- For every `--option_flag=<value>` CLI switch, a corresponding environment variable `OPTION_FLAG=<value>` exist and is used as default if set (**CLI switches always prevails**).
- A complete list of CLI options is available running benchmarks with the `--help` switch.

**Running a Subset of Benchmarks**

The `--benchmark_filter=<regex>` option (or `BENCHMARK_FILTER=<regex>` environment variable) can be used to only run the benchmarks that match the specified `<regex>`

```bash
./run_benchmarks.x --benchmark_filter=BM_memcpy/32
```

**Disabling Benchmarks**

It is possible to temporarily disable benchmarks by renaming the benchmark function to have the prefix `DISABLED_`. This will cause the benchmark to be skipped at runtime. For example:

```cpp
BENCHMARK(BM_fnc)
    ->Name("DISABLED_OriginalTestName")
    /*->...*/
```

---

## Using `RegisterBenchmark(name, fn, args...)`

The `RegisterBenchmark(name, func, args...)` function provides an alternative way to create and register benchmarks.

- `RegisterBenchmark(name, func, args...)` creates, registers, and returns a pointer to a new benchmark with the specified name that invokes `func(st, args...)` where `st` is a benchmark::State object.

- Unlike the `BENCHMARK` registration macros, which can **only be used at the global scope**, the `RegisterBenchmark` can be called anywhere. This allows for benchmark tests to be registered programmatically.

- Additionally `RegisterBenchmark` allows **any callable object to be registered as a benchmark**. Including capturing lambdas and function objects. For Example:

```cpp
auto BM_test = [](benchmark::State& st, auto Inputs) { /* ... */ };

int main(int argc, char** argv) {
  for (auto& test_input : {/* ... */})
    benchmark::RegisterBenchmark(test_input.name(), BM_test, test_input);
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
}
```

---

## Exiting with an Error

When errors caused by external influences, such as **file I/O and network communication**, occur within a benchmark the `State::SkipWithError(const char* msg)` function can be used to skip that run of benchmark and report the error.

- Note that **only future iterations of the `KeepRunning()` are skipped**.
- For the **ranged-for version of the benchmark loop Users must explicitly exit the loop**, otherwise all iterations will be performed. **Users may explicitly return to exit the benchmark immediately.**
- The `SkipWithError(...)` function may be used at any point within the benchmark, including before and after the benchmark loop. Moreover, if `SkipWithError(...)` has been used, it is **not required to reach the benchmark loop and one may return from the benchmark function early.**

```cpp
static void BM_test(benchmark::State& state) {
  auto resource = GetResource();
  if (!resource.good()) {
    state.SkipWithError("Resource is not good!");
    // KeepRunning() loop will not be entered.
  }
  while (state.KeepRunning()) {
    auto data = resource.read_data();
    if (!resource.good()) {
      state.SkipWithError("Failed to read data!");
      break;  // Needed to skip the rest of the iteration.
    }
    do_stuff(data);
  }
}

static void BM_test_ranged_fo(benchmark::State& state) {
  auto resource = GetResource();
  if (!resource.good()) {
    state.SkipWithError("Resource is not good!");
    return;  // Early return is allowed when SkipWithError() has been used.
  }
  for (auto _ : state) {
    auto data = resource.read_data();
    if (!resource.good()) {
      state.SkipWithError("Failed to read data!");
      break;  // REQUIRED to prevent all further iterations.
    }
    do_stuff(data);
  }
}
```

---

## Output Formats

The library supports multiple output formats.

- Use the `--benchmark_format=<console|json|csv>` flag (or set the `BENCHMARK_FORMAT=<console|json|csv>` environment variable) to set the format type.
- `console` is the default format.
- The Console format is intended to be a human readable format. By default the format generates color output. **Context is output on stderr and the tabular data on stdout**.
- The JSON format outputs human readable json split into two top level attributes. The **context** attribute contains information about the run in general, including information about the CPU and the date. The **benchmarks** attribute contains a list of every benchmark run.
- The CSV format outputs comma-separated values. The context is output on stderr and the CSV itself on stdout.

---

## Output Files

Write benchmark results to a file with the `--benchmark_out=<filename>` option (or set `BENCHMARK_OUT`).

- Specify the output format with `--benchmark_out_format={json|console|csv}` (or set `BENCHMARK_OUT_FORMAT={json|console|csv}`).
- Note that the 'csv' reporter is deprecated and the saved .csv file is not parsable by csv parsers.
- Specifying` --benchmark_out` does not suppress the console output.

---

## Extra Context

Sometimes it's useful to add extra context to the content printed before the results.

- By default this section includes information about the CPU on which the benchmarks are running.
- If you do want to add more context, you can use the `benchmark_context` command line flag:

```bash
$ ./run_benchmarks --benchmark_context=pwd=`pwd`
```

You can get the same effect with the API:

```cpp
benchmark::AddCustomContext("foo", "bar");
```

- Note that attempts to add a second value with the same key will fail with an error message.

---

## Side note: Use range-for loop instead of `KeepRunning`

- The reason the ranged-for loop is faster than using `KeepRunning`, is because `KeepRunning` requires a memory load and store of the iteration count ever iteration, whereas the ranged-for variant is able to keep the iteration count in a register.

```cpp
static void BM_Fast(benchmark::State &state) {
  for (auto _ : state) {
    FastOperation();
  }
}
BENCHMARK(BM_Fast);
```