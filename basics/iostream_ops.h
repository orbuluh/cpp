#pragma once
#include <gtest/gtest.h>

#include <ostream>
#include <string_view>

namespace iostream_ops {

std::vector<std::string> split(const std::string &str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        result.push_back(token);
    }
    return result;
}


TEST(IostreamOps, getlineForParseCsv) {
    std::string s{"a,b,c,e,f"};
    auto res = split(s, ',');
    EXPECT_EQ(5ul, res.size());
    EXPECT_EQ("a", res.front());
    EXPECT_EQ("f", res.back());
}

}  // namespace iostream_ops