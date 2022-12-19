#pragma once
#include <gtest/gtest.h>

namespace address_ops {

TEST(AddressOps, SizeOfPtr) {
  int a = 1;
  EXPECT_EQ(4, sizeof(a));
  EXPECT_EQ(8, sizeof(&a));  // size of pointer might or might not be the same
                             // as size of int.
}

template <typename T>
void ptrDiffTest() {
  // modified from discussion https://stackoverflow.com/a/12862408/4924135
  using TypeWithSameSzAsPtr = int64_t;
  static_assert(sizeof(TypeWithSameSzAsPtr) == sizeof(int*));

  T a{};
  T b{};
  T* ap = &a;
  T* bp = &b;

  // diff will be "how many T" difference between these 2 T*
  // it's actually of type: ptrdiff_t
  auto diff = bp - ap;
  EXPECT_EQ(1, static_cast<TypeWithSameSzAsPtr>(diff));

  // actual bytes difference need to be done by
  auto apInt = (TypeWithSameSzAsPtr)ap;
  auto bpInt = (TypeWithSameSzAsPtr)bp;
  auto pdiff = bpInt - apInt;
  EXPECT_EQ(sizeof(T), pdiff);  // 1 int is 4 bytes
  EXPECT_EQ(diff * sizeof(T), pdiff);
}

struct alignas(64) AlignedType {
  std::atomic<int> val;
};

TEST(AddressOps, DifferentTypeAddressDiff) {
  ptrDiffTest<int>();
  ptrDiffTest<int64_t>();
  ptrDiffTest<std::atomic<int>>();
  ptrDiffTest<AlignedType>();
}

}  // namespace address_ops