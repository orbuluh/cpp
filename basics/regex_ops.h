#pragma once
#include <gtest/gtest.h>
#include <regex>
#include <ostream>
//#include <format> // gcc-12 not supported yet

namespace regex_ops {

TEST(RegexOps, BasicGroupExtract) {
    const std::string text = "abcdef.txt";
    const std::regex base_regex("(\\w+)\\.(.*)");
    std::smatch base_match;
    EXPECT_TRUE(std::regex_match(text, base_match, base_regex));
    // The first sub_match is the whole string; the next
    // sub_match is the first parenthesized expression.
    EXPECT_EQ(3, base_match.size());
    EXPECT_EQ("abcdef.txt", base_match[0].str()); // type is std::ssub_match
    EXPECT_EQ("abcdef", base_match[1].str());
    EXPECT_EQ("txt", base_match[2].str());
}

TEST(RegexOps, CsvExtract) {
    const std::string text = "123,21321,321";
    const std::regex base_regex("(\\d+),(\\d+),(\\d+)");
    std::smatch base_match;
    EXPECT_TRUE(std::regex_match(text, base_match, base_regex));
    // The first sub_match is the whole string; the next
    // sub_match is the first parenthesized expression.
    for (int i = 1; i < base_match.size(); ++i)
        std::cout << base_match[i].str() << '\n';
}




}  // namespace regex_ops