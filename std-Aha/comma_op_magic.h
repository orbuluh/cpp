#pragma once

#include <iostream>
#include <sstream>

namespace comma_op_magic {

void printWithRecursion() {
    std::cout << '\n';
}

template<typename T, typename... Remaining>
void printWithRecursion(const T& t, const Remaining& ...r) {
    std::cout << ' ' << t;
    printWithRecursion(r...);
}

template<typename... T>
void magicPrint(const T& ...t) {
    const int dummyListVal = 0; // just for clarity, can just put 0 below, as a return for std::initializer_list<int>
    // comma operator, (exprA, exprB):
    // execute exprA, discard result of exprA, execute exprB, return result of exprB
    (void)std::initializer_list<int>{ (std::cout << t << '\n', dummyListVal)... };
}

template<typename... T>
void magicPrintCsv(const T& ...t) {
    std::stringstream ss;
    bool noComma = true;
    (void)std::initializer_list<bool>{ ( ss << (noComma? "": ", ") << t, noComma = false)... };
    std::cout << ss.str() << '\n';
}

template<typename U, typename... V>
auto magicMin(const U&  u, const V& ...v) -> typename std::common_type<U, V...>::type {
    magicPrintCsv(u, v...);
    using RetT = typename std::common_type<U, V...>::type;
    auto result = static_cast<RetT>(u);
    const int dummyListVal = 0; // just for clarity, can just put 0 below, as a return for std::initializer_list<int>
    (void)std::initializer_list<int>{ ((v < result)? (result = static_cast<RetT>(v), dummyListVal): dummyListVal)... };
    std::cout << "min: " << result << std::endl;
    return result;
}

void demo() {
    printWithRecursion(1, "yo", 2.34, "huh", '?');
    magicPrint(3, 2, 1, "1234567");
    magicPrintCsv("Sen", "Pa", "Hoo");
    magicMin(1.234, 5u, -6.78999999999, 9999998898);
}

} // namespace comma_op_magic