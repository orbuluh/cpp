#pragma once

#include <iostream>
#include <chrono>

namespace decorator {

template<typename Functor, typename... Args>
void timeTheFunctor(Functor fnc, Args ...args) {
    using clock = std::chrono::high_resolution_clock;
    const auto t1 = clock::now();
    fnc(std::forward<Args>(args)...);
    const auto t2 = clock::now();
    std::cout << (t2 - t1).count() << "ns\n";
};

} // namespace decorator