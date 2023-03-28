// Modified from
// https://www.cs.cmu.edu/afs/cs/academic/class/15213-f05/code/mem/mountain

/* Find number of cycles used by function that takes 2 arguments */

/* Function to be tested takes two integer arguments */
typedef int (*TestFncT)(int, int);

/*
Parameters:
  k:  How many samples must be within epsilon for convergence
  epsilon: What is tolerance
  maxSamples: How many samples until give up?
*/
struct TestParams {
  bool shouldFlushCache = false;
  int k = 3;
  double epsilon = 0.01;
  int maxSamples = 20;
  bool compensate = false;
};

/* Compute time used by function f */
double fcyc2(TestFncT f, int param1, int param2, bool shouldFlushCache);
double fcyc2_full(TestFncT f, int param1, int param2,
                  const TestParams& params);


/*************  Use time of day (tod) as clocking methods *****************/

double fcyc2_tod(TestFncT f, int param1, int param2, bool shouldFlushCache);
double fcyc2_full_tod(TestFncT f, int param1, int param2,
                      const TestParams& params);
