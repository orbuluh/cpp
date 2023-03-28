// Modified from
// https://www.cs.cmu.edu/afs/cs/academic/class/15213-f05/code/mem/mountain


namespace cycle_counter {

/* Routines for using cycle counter */

/* Start the counter */
void start_counter();

/* Get # cycles since counter started */
double get_counter();

/* Measure overhead for counter */
double ovhd();

/* Determine clock rate of processor */
double mhz(int verbose);

/* Determine clock rate of processor, having more control over accuracy */
double mhz_full(int verbose, int sleepTime);

/** Special counters that compensate for timer interrupt overhead */

void start_comp_counter();

double get_comp_counter();

} // namespace cycle_counter