#include "cookbook.h"

#include <boost/any.hpp>
#include <iostream>
#include <vector>

namespace boost_any {
void demo() {
    std::vector<boost::any> some_values;
    some_values.push_back(10);
    some_values.push_back(std::string("Wow!"));

    std::string& s = boost::any_cast<std::string&>(some_values.back());
    s += " That is great!\n";
    std::cout << s;

    try {
        auto& s = boost::any_cast<int&>(some_values.back());
    } catch (boost::wrapexcept<boost::bad_any_cast>& e) {
        std::cout << "As expected, bad conversion to reference: " << e.what() << '\n';
    }

    // note - not boost::any_cast<int*> here.
    int* expectNull = boost::any_cast<int>(&some_values.back());
    std::cout << "As expected, bad conversion to ptr, return " << expectNull << std::endl;
}
} // namespace boost_any