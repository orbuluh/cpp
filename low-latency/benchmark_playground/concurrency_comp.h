#pragma once

#include <benchmark/benchmark.h>

#include <atomic>
#include <mutex>

// from Concurrency in C++: A Programmer’s Overview (part 2 of 2) - Fedor Pikus

namespace atomic_ops {

#define REPEAT2(x) \
  {x} { x }
#define REPEAT4(x) REPEAT2(x) REPEAT2(x)
#define REPEAT8(x) REPEAT4(x) REPEAT4(x)
#define REPEAT16(x) REPEAT8(x) REPEAT8(x)
#define REPEAT32(x) REPEAT16(x) REPEAT16(x)
#define REPEAT64(x) REPEAT32(x) REPEAT32(x)
#define REPEAT(x) REPEAT64(x)

#define ARGS \
  ->ThreadRange(8, 8)->UseRealTime()

std::atomic<unsigned long>* p(new std::atomic<unsigned long>);
unsigned long* q(new unsigned long);

static void BM_write_nonatomic(benchmark::State& state) {
  if (state.thread_index() == 0) {
    *q = 0;
  }
  for (auto _ : state) {
    REPEAT(benchmark::DoNotOptimize(*q = 1);)
  }
  state.SetItemsProcessed(64 * state.iterations());
}

BENCHMARK(BM_write_nonatomic) ARGS;

static void BM_write_atomic(benchmark::State& state) {
  if (state.thread_index() == 0) {
    *p = 0;
  }
  for (auto _ : state) {
    REPEAT(benchmark::DoNotOptimize(*p = 1);)
  }
  state.SetItemsProcessed(64 * state.iterations());
}

BENCHMARK(BM_write_atomic) ARGS;

static void BM_write_atomic_store(benchmark::State& state) {
  if (state.thread_index() == 0) {
    *p = 0;
  }
  for (auto _ : state) {
    REPEAT(p->store(42, std::memory_order_relaxed);)
  }
  state.SetItemsProcessed(64 * state.iterations());
}

BENCHMARK(BM_write_atomic_store) ARGS;

//---------------------------------------------------------------------

static void BM_increment_nonatomic(benchmark::State& state) {
  if (state.thread_index() == 0) {
    *q = 0;
  }
  for (auto _ : state) {
    REPEAT(benchmark::DoNotOptimize(++*q);)
  }
  state.SetItemsProcessed(64 * state.iterations());
}

BENCHMARK(BM_increment_nonatomic) ARGS;

static void BM_increment_atomic(benchmark::State& state) {
  if (state.thread_index() == 0) {
    *p = 0;
  }
  for (auto _ : state) {
    REPEAT(benchmark::DoNotOptimize(++*p);)
  }
  state.SetItemsProcessed(64 * state.iterations());
}

BENCHMARK(BM_increment_atomic) ARGS;

static void BM_fetch_add_atomic(benchmark::State& state) {
  if (state.thread_index() == 0) {
    *p = 0;
  }
  for (auto _ : state) {
    REPEAT(p->fetch_add(1);)
  }
  state.SetItemsProcessed(64 * state.iterations());
}

BENCHMARK(BM_fetch_add_atomic) ARGS;

//---------------------------------------------------------------------

#include "spinlock.h"
spinlock s;

static void BM_increment_spinlock(benchmark::State& state) {
  if (state.thread_index() == 0) {
    *q = 0;
  }
  for (auto _ : state) {
    REPEAT(std::lock_guard<spinlock> l(s); benchmark::DoNotOptimize(++*q);)
  }
  state.SetItemsProcessed(64 * state.iterations());
}

BENCHMARK(BM_increment_spinlock) ARGS;

std::mutex m;

static void BM_increment_std_mutex(benchmark::State& state) {
  if (state.thread_index() == 0) {
    *q = 0;
  }
  for (auto _ : state) {
    REPEAT(std::lock_guard<std::mutex> l(m); benchmark::DoNotOptimize(++*q);)
  }
  state.SetItemsProcessed(64 * state.iterations());
}

BENCHMARK(BM_increment_std_mutex) ARGS;

static void BM_increment_cas(benchmark::State& state) {
  if (state.thread_index() == 0) {
    *p = 0;
  }
  for (auto _ : state) {
    REPEAT(
      unsigned long xl = p->load(std::memory_order_relaxed);
      while (!p->compare_exchange_strong(xl, xl + 1)) {}
    )
  }
  state.SetItemsProcessed(64 * state.iterations());
}

BENCHMARK(BM_increment_cas) ARGS;

static void BM_increment_cas_weak(benchmark::State& state) {
  if (state.thread_index() == 0) {
    *p = 0;
  }
  for (auto _ : state) {
    REPEAT(
      unsigned long xl = p->load(std::memory_order_relaxed);
      while (!p->compare_exchange_weak(xl, xl + 1)) {}
    )
  }
  state.SetItemsProcessed(64 * state.iterations());
}

BENCHMARK(BM_increment_cas_weak) ARGS;


}  // namespace atomic_ops