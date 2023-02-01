#include <iostream>
namespace variadic::demo {

void print() { std::cout << '\n'; }

template<typename T, typename... Remaining>
void print(const T& t1, const Remaining& ...t) {
    std::cout << t1 << ' ';
    print(t...);
}

};