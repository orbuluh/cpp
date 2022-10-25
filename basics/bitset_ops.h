#pragma once
#include <gtest/gtest.h>

#include <bitset>
#include <climits>

namespace bitset_ops {

TEST(BitsetOps, Constructor) {
  // empty constructor ------------------------------------------------------
  std::bitset<8> b1;
  EXPECT_EQ("00000000", b1.to_string());
  EXPECT_TRUE(b1.none());

  // unsigned long long constructor -----------------------------------------
  std::bitset<8> b2(42);  // [0,0,1,0,1,0,1,0]

  // first 6 bits are 0 and remaining bits are 1
  std::bitset<70> bl(ULLONG_MAX);  // [0,0,0,0,0,0,1,1,1,...,1,1,1]
  EXPECT_EQ(64, bl.count());

  // only the right side f0 fits into the 8 bit bitset
  std::bitset<8> bs(0xfff0);  // [1,1,1,1,0,0,0,0]

  // string constructor -----------------------------------------------------
  std::string bit_string = "110010";
  // all 6 bits in the string fit in
  std::bitset<8> b3(bit_string);
  EXPECT_EQ("00110010", b3.to_string());

  // An optional starting position pos is provided
  std::bitset<8> b4(bit_string, 2);  // bit_string pos=2, it's "0010"
  EXPECT_EQ("00000010", b4.to_string());

  // An optional starting position pos and length n are provided
  std::bitset<8> b5(bit_string, 2, 3);  // bit_string pos=2, n=3 is "001"
  EXPECT_EQ("00000001", b5.to_string());

  // char* constructor using custom digits
  std::bitset<8> b6("XXXXYYYY", 8, 'X', 'Y');
  EXPECT_EQ("00001111", b6.to_string());
}

TEST(BitsetOps, Ops) {
  std::bitset<8> b1("00001111");
  std::bitset<8> b2("01010101");

  EXPECT_EQ("00000101", (b1 & b2).to_string());
  EXPECT_EQ("01011111", (b1 | b2).to_string());
  EXPECT_EQ("01011010", (b1 ^ b2).to_string());
  EXPECT_EQ("11110000", (~b1).to_string()); // no ! negate!
}

}  // namespace bitset_ops