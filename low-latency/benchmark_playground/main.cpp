//#include "stringCopy.h"
//#include "dynamicDispatch.h"
//#include "falseSharing.h"
//#include "cache_behavior_item_1.h"
//#include "cache_behavior_item_2.h"
#include "cache_behavior_item_3.h"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// NOTE: some of the benchmark is using large array. If you run and getting
// a segfault during array initialize, it's likely because stack limit is too
// small (default ~8MB), run `ulimit -s` to check, and run `ulimit -s unlimited`
// to loose the constraint!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


BENCHMARK_MAIN();