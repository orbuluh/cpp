#include <vector>
#include <sstream>
#include <iostream>
#include <numeric> //accumulate is here not in algorithm!
namespace std_vector_ops {

void print(std::string_view description, const std::vector<int>& vec) {
    std::ostringstream os;
    for (auto val : vec) {
        os << val << ' ';
    }
    std::cout << description << ": " << os.str() << '\n';
}

void reverseEraseAllEven() {
    std::vector<int> v {1, 2, 3, 4, 5, 6};
    print("original vec", v);
    auto it = v.end();
    while (it != v.begin()) {
        it--;
        auto oriVal = *it;
        if (oriVal % 2) {
            auto it2 = v.erase(it);
            //assert((it2 - v.begin()) == (it - v.begin()))
        }
        // can't insert with it, this position itself is invalid already
        // (but ++it, --it still point to its next/prev element, so the loop is fine)
        //v.insert(it, oriVal*2 + 1);
    }
    print("after erase odd element", v);
}

void accumulatePair() {
    std::vector<std::pair<int, int>> v{ {1, 5}, {4, 3}, {9, 10}, {2, 8}} ;
    const auto n = v.size();
    std::cout << "for the first half adding pair.first" << '\n';
    int totalCost = std::accumulate(v.begin(), v.begin() + n / 2, 0, [](int val, const std::pair<int, int>& p){
            std::cout << "adding " << p.first << '\n';
            return val + p.first;
    });
    std::cout << "for the first half adding pair.second" << '\n';
    totalCost = std::accumulate(v.begin() + n / 2, v.end(), totalCost, [](int val, const std::pair<int, int>& p){
        std::cout << "adding " << p.second << '\n';
        return val + p.second;
    });
    std::cout << totalCost << std::endl;
}

void demo() {
    //reverseEraseAllEven();
    accumulatePair();
}

} // namespace std_vector_ops