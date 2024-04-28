#include <gtest/gtest.h>

#include <string>

#include "ch3_typelists/typelists.hpp"

using namespace m_cpp_d::ch3;

TEST(Ch3Typelists_test, LengthTest) {
  using TestList = TYPELIST_3(int, double, float);
  static_assert(Length<TestList>::value == 3);
}
