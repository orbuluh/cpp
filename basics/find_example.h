#pragma once

#include <iostream>
#include <vector>

namespace find_example {

void demo() {
    std::vector<int> v{1, 3, 5, 7, 9};
    auto searchBegin = std::lower_bound(v.begin(), v.end(), 2);
    auto searchEnd = std::upper_bound(v.begin(), v.end(), 6);
    // itBegin == 3, itEnd == 7
    std::cout << "search range: [" << *searchBegin << ", " << *searchEnd << ")\n";
    auto bigNotInRangePointTo = std::find(searchBegin, searchEnd, 8);
    std::cout << *bigNotInRangePointTo << '\n'; // print 7, e.g. the itEnd, which is not v.end
    auto smallNotInRangePointTo = std::find(searchBegin, searchEnd, 2);
    std::cout << *smallNotInRangePointTo << '\n'; // print 7, e.g. the itEnd, which is not v.end

}

} // namespace find_example