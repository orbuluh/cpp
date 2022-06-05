#include <iostream>
namespace fold_expression {


bool allVar() {                                              // (1)
    return true;
}

template<typename T, typename ...Ts>                         // (2)
bool allVar(T t, Ts ... ts) {                                // (3)
    return t && allVar(ts...);                               // (4)
}

template<typename... Args>                                   // (5)
bool all(Args... args) { return (... && args); }

void demo() {
    std::cout << std::boolalpha;

    std::cout << '\n';

    std::cout << "allVar(): " << allVar() << '\n';
    std::cout << "all(): " << all() << '\n';

    std::cout << "allVar(true): " << allVar(true) << '\n';
    std::cout << "all(true): " << all(true) << '\n';

    std::cout << "allVar(true, true, true, false): " 
              << allVar(true, true, true, false) << '\n';
    std::cout << "all(true, true, true, false): " 
              << all(true, true, true, false) << '\n';

    std::cout << '\n';
}
}