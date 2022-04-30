#include "RaiiThread.h"

namespace managing::threads::examples {

void threadOutOfScopeFailDemo() {
    int someLocalState = 0;
    std::thread t([&](){
        for (int i = 0; i < 100000000; ++i) {
            someLocalState += i;
            someLocalState -= i;
        }
    });
    //someLocalState would have leak as function out of scope
}

void raiiThreadDemo() {
    int someLocalState = 0;
    joining_thread t([&](int count){
        for (int i = 0; i < count; ++i) {
            someLocalState += i;
            someLocalState -= i;
        }
    }, 100000000);
    // someLocalState won't leak as ~RaiiThread do the join
}

} // managing::threads::examples