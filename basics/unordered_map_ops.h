#pragma once
#include <gtest/gtest.h>
#include <unordered_map>

namespace unordered_map_ops {

TEST(UnorderedMapOps, ConstructorWithFullCustomization) {
    using KeyT = std::pair<int, int>;
    // Note that the initial bucket count has to be passed to the constructor
    int bucketCnt = 16;
    std::hash<int> h;
    auto myHash = [&](const KeyT& key) { return h(key.first) ^ h(key.second); };
    auto myEqual = [](const KeyT& k1, const KeyT& k2) { return k1.first == k2.first && k1.second == k2.second; };
    std::unordered_map<KeyT, int, decltype(myHash), decltype(myEqual)> mymap(bucketCnt, myHash, myEqual);
    // Because pair have equal define, you can also do:
    //std::unordered_map<KeyT, int, decltype(myHash)> mymap(bucketCnt, myHash);
    auto key = std::make_pair(1, 2);
    mymap[key] = 3;
    EXPECT_EQ(mymap[key], 3);
}

} // namespace unordered_map_ops