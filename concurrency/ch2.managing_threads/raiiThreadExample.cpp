#include "RaiiThread.h"

namespace managing::threads::examples {

void raiiThreadDemo() {
    int someLocalState = 0;
    RaiiThread t;
    t.start([&](){
        for (int i = 0; i < 100000000; ++i) {
            someLocalState += i;
            someLocalState -= i;
        }
    });
    // someLocalState won't leak as ~RaiiThread do the join
}

} // managing::threads::examples