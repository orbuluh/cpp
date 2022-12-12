#pragma once
#include <gtest/gtest.h>
#include <utility>

namespace utility_ops {

TEST(Utility, FibonacciWithExchange) {
  int a = 1;
  int b = 2;
  // old_val = exchange(new_val_to_be, new_val)
  // std::exchange(a, b): return a's original value and replace a with b
  // so fibonacci is achieved by assign b to a + b, and assign a to b
  int tmp = std::exchange(b, std::exchange(a, b) + b);
  EXPECT_EQ(2, a);
  EXPECT_EQ(3, b);
  EXPECT_EQ(tmp, a); // a is replaced with b's old value, which equal to tmp
}

} // namespace utility_ops