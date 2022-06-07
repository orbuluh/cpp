#include <vector>
#include <sstream>
#include <iostream>
namespace structural_binding {

void simpleSyntax() {
    std::pair<int, double> p{1, 2.0};
    //---------------------------------------------
    auto& [x, y] = p;
    std::cout << x << ' ' << y << '\n'; // 1 2
    x = 3;
    y = 0.14;
    std::cout << p.first << ' ' << p.second << '\n'; // 3 0.14
    //---------------------------------------------
    auto [a, b] = p;
    std::cout << a << ' ' << b << '\n'; // 3 0.14
    a = 5;
    b = 6.78;
    std::cout << p.first << ' ' << p.second << '\n'; // 3 0.14
    //---------------------------------------------
    auto&& [m, n] = p;
    std::cout << m << ' ' << n << '\n'; // 3 0.14
    m = 2;
    n = 0.12;
    std::cout << p.first << ' ' << p.second << '\n'; // 2 0.12
}

void unorderedMapUseCase() {
    std::unordered_map<std::string, int> idxes;
    auto [it, inserted] = idxes.insert({"A", 0});
    std::cout << "inserted=" << inserted << " key=" << it->first << " val=" << it->second << '\n';
    it->second = 100;
    std::cout << idxes.at("A") << '\n'; // 100!

    auto [it2, inserted2] = idxes.insert({"B", 3});

    for (auto& [key, val] : idxes) {
        std::cout << key << ' ' << val << '\n';
        val *= -1;
    }

    for (const auto& [key, val] : idxes) {
        std::cout << key << ' ' << val << '\n';
    }

}


void demo() {
    //simpleSyntax();
    unorderedMapUseCase();
}

} // namespace structural_binding