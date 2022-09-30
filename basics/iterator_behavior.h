#pragma once
#include <gtest/gtest.h>
#include <vector>

namespace iterator_behavior {

TEST(IteratorBehavior, Basics) {
    std::vector<int> vec{1, 4, 8};
    auto it = vec.begin();
    EXPECT_EQ(*it, 1);
    EXPECT_EQ(it - vec.begin(), 0);
    // NOTE: std::prev(bec.begin) is UB!!!!!

    std::advance(it, 2);
    EXPECT_EQ(*it, 8);
    std::advance(it, -1);
    EXPECT_EQ(*it, 4);

    it = vec.end();
    EXPECT_EQ(*std::prev(it), 8);
    EXPECT_EQ(it - vec.begin(), vec.size());
}

TEST(IteratorBehavior, VecInsert) {
    std::vector<int> vec{1, 4, 8};
    auto it = std::lower_bound(vec.begin(), vec.end(), 3);
    EXPECT_EQ(*it, 4);
    auto it2 = vec.insert(it, 3);
    // iterators may not be valid in insert or delete operation as memory
    // location could have changed, so you should not try to access it, nor
    // std::prev, std::next etc ... SAME for erase!
    // EXPECT_EQ(*it, 4);  //BAD
    // EXPECT_EQ(*(std::next(it)), 8); //BAD
    //EXPECT_EQ(*(std::prev(it)), 3); //BAD
    EXPECT_EQ(*it2, 3);
    EXPECT_EQ(*std::prev(it2), 1);
    EXPECT_EQ(*std::next(it2), 4);
}

TEST(IteratorBehavior, VecErase) {
    std::vector<int> vec{1, 4, 8};
    auto it = std::lower_bound(vec.begin(), vec.end(), 4);
    EXPECT_EQ(*it, 4);

    // erase return the iterator point to the next element!
    auto it2 = vec.erase(it);

    EXPECT_EQ(*it2, 8);
    EXPECT_EQ(*std::prev(it2), 1);
    EXPECT_EQ(std::next(it2), vec.end());
}


} // namespace iterator_behavior