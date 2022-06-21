#include "event_loop.h"
#include <iostream>
namespace event_loop {
void demo() {
    {
        EventLoop eventLoop;

        eventLoop.enqueue(
            [] { std::cout << "message from a different thread\n"; });

        std::cout << "prints before or after the message above\n";

        std::cout << eventLoop.enqueueSync(
            [](const int& x, int&& y, int z) { return x + y + z; }, 1, 2, 3) << '\n';

        std::future<int> result =
            eventLoop.enqueueAsync([](int x, int y) { return x + y; }, 1, 2);
        //
        // do some heavy work here
        //
        using namespace std::literals;
        std::this_thread::sleep_for(1s);
        std::cout << result.get() << '\n';
    }

    std::cout << "guaranteed to be printed the last\n";
}
} // namespace event_loop