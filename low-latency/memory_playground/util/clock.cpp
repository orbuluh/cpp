// Modified from
// https://www.cs.cmu.edu/afs/cs/academic/class/15213-f05/code/mem/mountain

#include "clock.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <unistd.h>

#include <array>
#include <cstdint>

namespace cycle_counter {

/* Routines for using cycle counter */

/* Detect whether running on Alpha */
#ifdef __alpha
#define IS_ALPHA 1
#else
#define IS_ALPHA 0
#endif

/* Detect whether running on x86 */
#if defined(__i386__) || defined(__x86_64__)
#define IS_x86 1
#else
#define IS_x86 0
#endif

/* Keep track of most recent reading of cycle counter */
static unsigned cyc_hi = 0;
static unsigned cyc_lo = 0;

#if IS_ALPHA

/* Use Alpha cycle timer to compute cycles.
   Then use measured clock speed to compute seconds */

/*
 * counterRoutine is an array of Alpha instructions to access the Alpha's
 * processor cycle counter. It uses the rpcc instruction to access the counter.
 *
 * This 64 bit register is divided into two parts. The lower 32 bits are the
 * cycles used by the current process. The upper 32 bits are wall lock cycles.
 *
 * These instructions read the counter, and convert the lower 32 bits into an
 * unsigned int - this is the user space counter value.
 *
 * NOTE: The counter has a very limited time span. With a 450MhZ clock the
 * counter can time things for about 9 seconds.
 */

constexpr std::array<uint32_t, 3> counterRoutine{0x601fc000u, 0x401f0000u,
                                                 0x6bfa8001u};
auto counter = reinterpret_cast<std::uint32_t (*)()>(counterRoutine.data());

void start_counter() {
  /* Get cycle counter */
  cyc_hi = 0;
  cyc_lo = counter();
}

double get_counter() {
  unsigned ncyc_hi, ncyc_lo;
  unsigned hi, lo, borrow;
  double result;
  ncyc_lo = counter();
  ncyc_hi = 0;
  lo = ncyc_lo - cyc_lo;
  borrow = lo > ncyc_lo;
  hi = ncyc_hi - cyc_hi - borrow;
  result = (double)hi * (1 << 30) * 4 + lo;
  if (result < 0) {
    fprintf(stderr, "Error: Cycle counter returning negative value: %.0f\n",
            result);
  }
  return result;
}
#endif /* Alpha */

#if IS_x86
void access_counter(unsigned* hi, unsigned* lo) {
  /* Get cycle counter

  Uses the rdtsc instruction to read the current value of the TSC register and
  store it in the edx and eax registers. It then uses the movl instruction to
  move the values of edx and eax into the memory locations pointed to by the
  hi and lo pointers, respectively.

  The asm keyword is used to insert inline assembly code into the C++ code. The
  first argument to asm is a string that contains the assembly code to be
  executed. The %0 and %1 placeholders in the assembly code are replaced by the
  values of the hi and lo pointers, respectively.

  The =r and "%edx", "%eax" expressions are used as input and output operands to
  indicate which registers are used and modified by the assembly code.
  */
  asm("rdtsc; movl %%edx,%0; movl %%eax,%1"
      : "=r"(*hi), "=r"(*lo)
      : /* No input */
      : "%edx", "%eax");
}

void start_counter() { access_counter(&cyc_hi, &cyc_lo); }

double get_counter() {
  unsigned ncyc_hi, ncyc_lo;
  unsigned hi, lo, borrow;
  double result;
  /* Get cycle counter */
  access_counter(&ncyc_hi, &ncyc_lo);
  /* Do double precision subtraction */
  lo = ncyc_lo - cyc_lo;
  borrow = lo > ncyc_lo;
  hi = ncyc_hi - cyc_hi - borrow;
  result = (double)hi * (1 << 30) * 4 + lo;
  if (result < 0) {
    fprintf(stderr, "Error: Cycle counter returning negative value: %.0f\n",
            result);
  }
  return result;
}
#endif /* x86 */

double ovhd() {
  /* Do it twice to eliminate cache effects */
  double result;
  for (int i = 0; i < 2; i++) {
    start_counter();
    result = get_counter();
  }
  return result;
}

/* Determine clock rate by measuring cycles
   elapsed while sleeping for sleepTime seconds */
double mhz_full(int verbose, int sleepTime) {
  double rate;
  start_counter();
  sleep(sleepTime);
  rate = get_counter() / (1e6 * sleepTime);
  if (verbose) printf("Processor Clock Rate ~= %.1f MHz\n", rate);
  return rate;
}

/* Version using a default sleepTime */
double mhz(int verbose) { return mhz_full(verbose, 2); }

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
  start_counter();
  oldt = get_counter();
  while (e < NEVENT) {
    double newt = get_counter();
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

void start_comp_counter() {
  struct tms t;
  if (cyc_per_tick == 0.0) callibrate(1);
  times(&t);
  start_tick = t.tms_utime;
  start_counter();
}

double get_comp_counter() {
  double time = get_counter();
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

} // namespace cycle_counter