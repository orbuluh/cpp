#include <vector>
#include <sstream>
#include <iostream>

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

void demo() {
    reverseEraseAllEven();
}

} // namespace std_vector_ops