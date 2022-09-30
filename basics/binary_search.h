#pragma once

#include <gtest/gtest.h>
#include <algorithm>
#include <vector>
#include <iostream>

namespace std_binary_search {

TEST(IteratorBehavior, UpperBoundLocation) {
    // upper_bound: finding first index with snapIdx > target
    std::vector<int> vec{1, 4, 8};

    auto it = std::upper_bound(vec.begin(), vec.end(), 0);
    EXPECT_EQ(*it, 1);
    EXPECT_EQ(it, vec.begin());

    it = std::upper_bound(vec.begin(), vec.end(), 3);
    EXPECT_EQ(*it, 4);

    it = std::upper_bound(vec.begin(), vec.end(), 5);
    EXPECT_EQ(*it, 8);

    it = std::upper_bound(vec.begin(), vec.end(), 9);
    EXPECT_EQ(it, vec.end());
}

TEST(IteratorBehavior, LowerBoundLocation) {
    // lower_bound: finding first index with snapIdx >= target
    std::vector<int> vec{1, 4, 8};

    auto it = std::lower_bound(vec.begin(), vec.end(), 0);
    EXPECT_EQ(*it, 1);
    EXPECT_EQ(it, vec.begin());

    it = std::lower_bound(vec.begin(), vec.end(), 1);
    EXPECT_EQ(*it, 1);

    it = std::lower_bound(vec.begin(), vec.end(), 3);
    EXPECT_EQ(*it, 4);

    it = std::lower_bound(vec.begin(), vec.end(), 5);
    EXPECT_EQ(*it, 8);

    it = std::lower_bound(vec.begin(), vec.end(), 8);
    EXPECT_EQ(*it, 8);

    it = std::lower_bound(vec.begin(), vec.end(), 9);
    EXPECT_EQ(it, vec.end());
}

TEST(BinarySearch, Behavior) {
    std::vector<int> vec{1, 4, 8};

    EXPECT_TRUE(std::binary_search(vec.begin(), vec.end(), 1));
    EXPECT_TRUE(std::binary_search(vec.begin(), vec.end(), 4));
    EXPECT_TRUE(std::binary_search(vec.begin(), vec.end(), 8));

    EXPECT_FALSE(std::binary_search(vec.begin(), vec.end(), 0));
    EXPECT_FALSE(std::binary_search(vec.begin(), vec.end(), 2));
    EXPECT_FALSE(std::binary_search(vec.begin(), vec.end(), 3));
    EXPECT_FALSE(std::binary_search(vec.begin(), vec.end(), 5));
    EXPECT_FALSE(std::binary_search(vec.begin(), vec.end(), 7));
    EXPECT_FALSE(std::binary_search(vec.begin(), vec.end(), 9));
}

} // namespace std_binary_search