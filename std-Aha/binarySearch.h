#pragma once

#include <algorithm>
#include <vector>
#include <iostream>

namespace std_binary_search {

struct SnapData {
    int snapIdx;
    int val;
};

void demo() {
    std::vector<SnapData> snapVec {{0, 10}, {2, 5}, {3, 7}, {3, 6}, {8, 4}};
    std::cout << "binary_search" << std::endl;
    // binary search with snapIdx, index 1 is not int the vec, print 0
    std::cout << std::binary_search(snapVec.begin(), snapVec.end(), SnapData{1, 4},
                                    [](const SnapData& d1, const SnapData& d2) { return d1.snapIdx < d2.snapIdx; }) << '\n';

    // binary search with val, val 7 is not int the vec, print 1
    std::cout << std::binary_search(snapVec.begin(), snapVec.end(), SnapData{2, 7},
                                    [](const SnapData& d1, const SnapData& d2) { return d1.val < d2.val; }) << '\n';

    auto it = snapVec.begin();
    std::cout << "\nlower_bound" << std::endl;
    auto lowerComp = [](const SnapData& d1, int tgt) { return d1.snapIdx < tgt; };

    // finding first index with snapIdx >= target
    it = std::lower_bound(snapVec.begin(), snapVec.end(), 1, lowerComp);
    std::cout << (it - snapVec.begin()) << std::endl; // print index 1

    it = std::lower_bound(snapVec.begin(), snapVec.end(), 2, lowerComp);
    std::cout << (it - snapVec.begin()) << std::endl; // print index 1

    it = std::lower_bound(snapVec.begin(), snapVec.end(), 3, lowerComp);
    std::cout << (it - snapVec.begin()) << std::endl; // print index 2

    it = std::lower_bound(snapVec.begin(), snapVec.end(), 99, lowerComp);
    std::cout << (it - snapVec.begin()) << std::endl; // print end (index 5)

    std::cout << "\nupper_bound" << std::endl;
    auto upperComp = [](int tgt, const SnapData& d1) { return tgt < d1.snapIdx ; };

    // finding first index with snapIdx > target
    it = std::upper_bound(snapVec.begin(), snapVec.end(), 1, upperComp);
    std::cout << (it - snapVec.begin()) << std::endl; // print index 2

    it = std::upper_bound(snapVec.begin(), snapVec.end(), 2, upperComp);
    std::cout << (it - snapVec.begin()) << std::endl; // print index 2

    it = std::upper_bound(snapVec.begin(), snapVec.end(), 3, upperComp);
    std::cout << (it - snapVec.begin()) << std::endl; // print index 4

    it = std::upper_bound(snapVec.begin(), snapVec.end(), 99, upperComp);
    std::cout << (it - snapVec.begin()) << std::endl; // print end (index 5)
}

} // namespace std_binary_search