/* Generate the memory mountain.

Modified from
https://www.cs.cmu.edu/afs/cs/academic/class/15213-f05/code/mem/mountain

*/

#include <stdio.h>
#include <stdlib.h>
#
#include "util/clock.h" /* routines to access the cycle counter */
#include "util/fcyc2.h" /* measurement routines */

static constexpr auto MINBYTES = (1 << 14); /* First working set size */
static constexpr auto MAXBYTES = (1 << 27); /* Last working set size */
static constexpr auto MAXSTRIDE = 15;       /* Stride x8 bytes */
static constexpr auto MAXELEMS = MAXBYTES / sizeof(long);

namespace memory_mountain {

long data[MAXELEMS]; /* The global array we'll be traversing */

int memReadTest(int numOfElems, int stride);
double measureThroughput(int size, int stride, double Mhz);

void demo() {
  for (int i = 0; i < MAXELEMS; i++) {
    data[i] = i;
  }

  const double Mhz = cycle_counter::mhz(0); /* Estimate the clock frequency */

  /* Not shown in the text */
  printf("Clock frequency is approx. %.1f MHz\n", Mhz);
  printf("Memory mountain (MB/sec)\n");

  printf("\t");
  for (int stride = 1; stride <= MAXSTRIDE; stride++) {
    printf("s%d\t", stride);
  }
  printf("\n");

  for (int size = MAXBYTES; size >= MINBYTES; size >>= 1) {
    /* Not shown in the text */
    if (size > (1 << 20))
      printf("%dm\t", size / (1 << 20));
    else
      printf("%dk\t", size / 1024);

    for (int stride = 1; stride <= MAXSTRIDE; stride++) {
      printf("%.0f\t", measureThroughput(size, stride, Mhz));
    }
    printf("\n");
  }
}

/*
 * generates the read sequence by scanning the first `numOfElems` elements of an
 * array with a `stride` of stride, using 4x4 loop unrolling.
 */

int memReadTest(int numOfElems, int stride) {
  long i = 0;
  const auto sx2 = stride * 2;
  const auto sx3 = stride * 3;
  const auto sx4 = stride * 4;
  const long limit = numOfElems - sx4;
  long acc0 = 0, acc1 = 0, acc2 = 0, acc3 = 0;

  /* Combine 4 elements at a time */
  for (i = 0; i < limit; i += sx4) {
    acc0 += data[i];
    acc1 += data[i + stride];
    acc2 += data[i + sx2];
    acc3 += data[i + sx3];
  }

  /* Finish any remaining elements */
  for (; i < numOfElems; i += stride) {
    acc0 += data[i];
  }
  return ((acc0 + acc1) + (acc2 + acc3));
}

/* run - Run memReadTest(numOfElems, stride) and return read throughput (MB/s).
 *  `size` is in bytes
 *  `stride` is in array elements
 *  Mhz is CPU clock frequency in Mhz.
 *
 *  Expect that:
 *  - Smaller values of `size` result in a smaller working set size, and thus
 *    better temporal locality.
 *  - Smaller values of `stride` result in better spatial locality.
 */
double measureThroughput(int size, int stride, double Mhz) {
  const int numOfElems = size / sizeof(double);

  memReadTest(numOfElems, stride); /* Warm up the cache */  // line:mem:warmup

  constexpr bool clearCache = false;

  double cycles = fcyc2(memReadTest, numOfElems, stride, clearCache);
  /* Call memReadTest(numOfElems,stride) */  // line:mem:fcyc
  return (size / stride) / (cycles / Mhz);
  /* Convert cycles to MB/s */  // line:mem:bwcompute
}

}  // namespace memory_mountain
