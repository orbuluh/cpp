#include "event_loop/demo.h"
#include "CIA/ch2.managing_threads/examples.h"
#include "demo/memory_order.h"
#include "demo/jthread_example.h"
#include "demo/thread_is_heavy.h"

int main() {
    //event_loop::demo();
    //managing::threads::examples::parallelAccumulateDemo();
    //managing::threads::examples::threadOutOfScopeFailDemo();
    //managing::threads::examples::raiiThreadDemo();
    //memory_order::demo();
    //memory_order_sync_with_atomic::demo();
    //memory_fence_producer_consumer::demo();
    //memory_order_relaxed_counter::demo();
    //jthread_example::demo();
    thread_is_heavy::demo();
}
