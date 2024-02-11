#include <gtest/gtest.h>

#include "util/utility.h"

TEST(utility_test, get_value) { EXPECT_EQ(util::get_value(), 42); }

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}