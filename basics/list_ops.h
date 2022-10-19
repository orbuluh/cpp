#pragma once
#include <gtest/gtest.h>
#include <list>

namespace list_ops {

void printList(const std::list<int>& li) {
    for (auto it = li.begin(); it != li.end(); ++it) {
        std::cout << *it << '\n';
    }
}

TEST(ListOps, Basics) {
    std::list<int> l1{1, 4, 8, 16, 32};
    //printList(l1);
    EXPECT_EQ(*l1.begin(), 1);
    EXPECT_EQ(*l1.rbegin(), 32);
    EXPECT_EQ(l1.back(), 32);

    auto l2 = l1; // make a copy

    l1.reverse(); // 32 16 8 4 1
    EXPECT_EQ(*l1.begin(), 32);
    EXPECT_EQ(*l1.rbegin(), 1);
    EXPECT_EQ(l1.back(), 1);

    // 4 1
    l1.remove_if([](int element) { return element % 8 == 0; });
    EXPECT_EQ(*l1.begin(), 4);
    EXPECT_EQ(*l1.rbegin(), 1);

    // 16 4 1
    l1.insert(l1.begin(), 16);
    EXPECT_EQ(*l1.begin(), 16);
    EXPECT_EQ(*l1.rbegin(), 1);

    l1.splice(std::next(l1.begin()), l2);
    // 16 1 4 8 16 32 1
    //    |---------|    l2 insert at first position of s1
    EXPECT_EQ(*l1.begin(), 16);
    EXPECT_EQ(*(++l1.begin()), 1);
    EXPECT_EQ(*l1.rbegin(), 1);
}

} // namespace list_ops