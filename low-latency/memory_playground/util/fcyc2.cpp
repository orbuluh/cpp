// Modified from
// https://www.cs.cmu.edu/afs/cs/academic/class/15213-f05/code/mem/mountain

/* Compute time used by a function f that takes two integer args */
#include "fcyc2.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>

#include "clock.h"

static double *values = nullptr;
int sampleCount = 0;
double *samples = nullptr;

constexpr bool KEEP_SAMPLES = true;
constexpr bool KEEP_VALS = true;
constexpr bool DEBUG = false;

/* Start new sampling process */
static void init_sampler(int k, int maxSamples) {
  if (values) free(values);
  values = static_cast<double *>(calloc(k, sizeof(double)));
  if constexpr (KEEP_SAMPLES) {
    if (samples) free(samples);
    /* Allocate extra for wraparound analysis */
    samples = static_cast<double *>(calloc(maxSamples + k, sizeof(double)));
  }
  sampleCount = 0;
}

/* Add new sample.  */
void add_sample(double val, int k) {
  int pos = 0;
  if (sampleCount < k) {
    pos = sampleCount;
    values[pos] = val;
  } else if (val < values[k - 1]) {
    pos = k - 1;
    values[pos] = val;
  }
  if constexpr (KEEP_SAMPLES) {
    samples[sampleCount] = val;
  }
  sampleCount++;
  /* Insertion sort */
  while (pos > 0 && values[pos - 1] > values[pos]) {
    double temp = values[pos - 1];
    values[pos - 1] = values[pos];
    values[pos] = temp;
    pos--;
  }
}

/* Get current minimum */
double get_min() { return values[0]; }

/* What is relative error for kth smallest sample */
double err(int k) {
  if (sampleCount < k) return 1000.0;
  return (values[k - 1] - values[0]) / values[0];
}

/* Have k minimum measurements converged within epsilon? */
int has_converged(int k_arg, double epsilon_arg, int maxSamples) {
  if ((sampleCount >= k_arg) &&
      ((1 + epsilon_arg) * values[0] >= values[k_arg - 1])) {
    return sampleCount;
  }
  if ((sampleCount >= maxSamples)) return -1;
  return 0;
}

/* Code to clear cache

the stuff array is likely intended to represent a working set of data that is
larger than the size of the CPU cache. By using a size of 2^17 elements, the
working set can exceed the size of most L1 and L2 caches, but still fit within
the capacity of many L3 caches.

Additionally, the stride of 8 used in the loop that accesses the stuff array can
help to ensure that the working set is spread out over multiple cache lines,
increasing the likelihood that it will be stored in the cache.
*/

/* Pentium III has 512K L2 cache, which is 128K ints */
constexpr auto ASIZE = (1 << 17);
/* Cache block size is 32 bytes */
constexpr auto STRIDE = 8;
static int stuff[ASIZE];
static int sink;

static void flushCache() {
  int x = sink;
  int i;
  for (i = 0; i < ASIZE; i += STRIDE) x += stuff[i];
  /*
  Reason of doing `sink = x`:

  It's possible that this is simply a way to prevent the compiler from
  optimizing away the loop, since the value of x is used after the loop in a way
  that the compiler cannot easily predict or optimize.
  */
  sink = x;
}

static void debugPrint(int k) {
  if constexpr (DEBUG) {
    printf(" %d smallest values: [", k);
    for (int i = 0; i < k; i++)
      printf("%.0f%s", values[i], i == k - 1 ? "]\n" : ", ");
  }
}

double fcyc2_full(test_funct testFnc, int param1, int param2,
                  bool shouldFlushCache, int k, double epsilon, int maxSamples,
                  bool compensate) {
  double result;
  init_sampler(k, maxSamples);
  if (compensate) {
    do {
      double cyc;
      if (shouldFlushCache) flushCache();
      testFnc(param1, param2); /* warm cache */
      cycle_counter::start_comp_counter();
      testFnc(param1, param2);
      cyc = cycle_counter::get_comp_counter();
      add_sample(cyc, k);
    } while (!has_converged(k, epsilon, maxSamples) &&
             sampleCount < maxSamples);
  } else {
    do {
      double cyc;
      if (shouldFlushCache) flushCache();
      testFnc(param1, param2); /* warm cache */
      cycle_counter::start_counter();
      testFnc(param1, param2);
      cyc = cycle_counter::get_counter();
      add_sample(cyc, k);
    } while (!has_converged(k, epsilon, maxSamples) &&
             sampleCount < maxSamples);
  }
  debugPrint(k);
  result = values[0];
  if constexpr (!KEEP_VALS) {
    free(values);
    values = nullptr;
  }
  return result;
}

double fcyc2(test_funct testFnc, int param1, int param2,
             bool shouldFlushCache) {
  return fcyc2_full(testFnc, param1, param2, shouldFlushCache, 3, 0.01, 300,
                    false);
}

/******************* Version that uses gettimeofday *************/

static double Mhz = 0.0;

#include <sys/time.h>

static struct timeval tstart;

/* Record current time */
void start_counter_tod() {
  if (Mhz == 0) Mhz = cycle_counter::mhz_full(0, 10);
  gettimeofday(&tstart, nullptr);
}

/* Get number of seconds since last call to start_timer */
double get_counter_tod() {
  struct timeval tfinish;
  long sec, usec;
  gettimeofday(&tfinish, nullptr);
  sec = tfinish.tv_sec - tstart.tv_sec;
  usec = tfinish.tv_usec - tstart.tv_usec;
  return (1e6 * sec + usec) * Mhz;
}

/** Special counters that compensate for timer interrupt overhead */

static double cyc_per_tick = 0.0;

#define NEVENT 100
#define THRESHOLD 1000
#define RECORDTHRESH 3000

/* Attempt to see how much time is used by timer interrupt */
static void callibrate(int verbose) {
  double oldt;
  struct tms t;
  clock_t oldc;
  int e = 0;
  times(&t);
  oldc = t.tms_utime;
  start_counter_tod();
  oldt = get_counter_tod();
  while (e < NEVENT) {
    double newt = get_counter_tod();
    if (newt - oldt >= THRESHOLD) {
      clock_t newc;
      times(&t);
      newc = t.tms_utime;
      if (newc > oldc) {
        double cpt = (newt - oldt) / (newc - oldc);
        if ((cyc_per_tick == 0.0 || cyc_per_tick > cpt) && cpt > RECORDTHRESH)
          cyc_per_tick = cpt;
        /*
        if (verbose)
          printf("Saw event lasting %.0f cycles and %d ticks.  Ratio = %f\n",
                 newt-oldt, (int) (newc-oldc), cpt);
        */
        e++;
        oldc = newc;
      }
      oldt = newt;
    }
  }
  if (verbose) printf("Setting cyc_per_tick to %f\n", cyc_per_tick);
}

static clock_t start_tick = 0;

void start_comp_counter_tod() {
  struct tms t;
  if (cyc_per_tick == 0.0) callibrate(0);
  times(&t);
  start_tick = t.tms_utime;
  start_counter_tod();
}

double get_comp_counter_tod() {
  double time = get_counter_tod();
  double ctime;
  struct tms t;
  clock_t ticks;
  times(&t);
  ticks = t.tms_utime - start_tick;
  ctime = time - ticks * cyc_per_tick;
  /*
  printf("Measured %.0f cycles.  Ticks = %d.  Corrected %.0f cycles\n",
         time, (int) ticks, ctime);
  */
  return ctime;
}

double fcyc2_full_tod(test_funct testFnc, int param1, int param2,
                      bool shouldFlushCache, int k, double epsilon,
                      int maxSamples, bool compensate) {
  double result;
  init_sampler(k, maxSamples);
  if (compensate) {
    do {
      double cyc;
      if (shouldFlushCache) flushCache();
      start_comp_counter_tod();
      testFnc(param1, param2);
      cyc = get_comp_counter_tod();
      add_sample(cyc, k);
    } while (!has_converged(k, epsilon, maxSamples) &&
             sampleCount < maxSamples);
  } else {
    do {
      double cyc;
      if (shouldFlushCache) flushCache();
      start_counter_tod();
      testFnc(param1, param2);
      cyc = get_counter_tod();
      add_sample(cyc, k);
    } while (!has_converged(k, epsilon, maxSamples) &&
             sampleCount < maxSamples);
  }
  debugPrint(k);
  result = values[0];
  if constexpr (!KEEP_VALS) {
    free(values);
    values = nullptr;
  }
  return result;
}

double fcyc2_tod(test_funct testFnc, int param1, int param2,
                 bool shouldFlushCache) {
  return fcyc2_full_tod(testFnc, param1, param2, shouldFlushCache, 3, 0.01, 20,
                        false);
}
