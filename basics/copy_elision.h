#pragma once

#include "util/noisy_pod.h"
#include <gtest/gtest.h>
#include <list>

namespace copy_elision {

// only one call to default constructor of T
NoisyPod returnOfPrValue() { return NoisyPod{}; }

//NoisyPod f2() { return f1(); }
//NoisyPod f3() { return f2(); }

class CopyElision : public testing::Test {
  void SetUp() override { NoisyPod::resetCounter(); }
  void TearDown() override { std::cout << "---end of scope---\n"; }
};

struct PotentialOverlapPod : NoisyPod
{
    // no elision when initializing a base-class subobject
    PotentialOverlapPod() : NoisyPod(returnOfPrValue()) {}
    // no elision because the D object being initialized might
    // be a base-class subobject of some other class*/
    PotentialOverlapPod(int) : PotentialOverlapPod(PotentialOverlapPod{}) {}
};

TEST_F(CopyElision, NormalConstruct) {
  auto res = NoisyPod{};
  EXPECT_EQ(1, NoisyPod::dftCtorCnt);
  EXPECT_EQ(0, NoisyPod::cpyCtorCnt);
  EXPECT_EQ(0, NoisyPod::cpyAssignCnt);
  EXPECT_EQ(0, NoisyPod::moveCtorCnt);  // 0, not 1, compare: with below case:
                    // InitializerIsPrvalueOfSameTypeMustCEUnlessPotentialSlice
}

TEST_F(CopyElision, ReturnOfPrValueMustCE) {
  auto res = returnOfPrValue();
  EXPECT_EQ(1, NoisyPod::dftCtorCnt);
  EXPECT_EQ(0, NoisyPod::cpyCtorCnt);
  EXPECT_EQ(0, NoisyPod::cpyAssignCnt);
  EXPECT_EQ(0, NoisyPod::moveCtorCnt);
}

TEST_F(CopyElision, InitializerIsPrvalueOfSameTypeMustCE) {
  auto res = NoisyPod(NoisyPod(returnOfPrValue()));
  EXPECT_EQ(1, NoisyPod::dftCtorCnt); // Not 3!
  EXPECT_EQ(0, NoisyPod::cpyCtorCnt);
  EXPECT_EQ(0, NoisyPod::cpyAssignCnt);
  EXPECT_EQ(0, NoisyPod::moveCtorCnt);
}

TEST_F(CopyElision, InitializerIsPrvalueOfSameTypeMustCEUnlessPotentialSlice) {
  auto res = PotentialOverlapPod{};
  EXPECT_EQ(1, NoisyPod::dftCtorCnt);
  EXPECT_EQ(0, NoisyPod::cpyCtorCnt);
  EXPECT_EQ(0, NoisyPod::cpyAssignCnt);
  EXPECT_EQ(1, NoisyPod::moveCtorCnt); // Not 0! Temporary is created and moved
                                       // Compare to NormalConstruct test case
}


/*

TEST_F(CopyElision, ReturnValueOptimization) {
  auto lt = f3();
  EXPECT_EQ(1, NoisyPod::dftCtorCnt);
  EXPECT_EQ(0, NoisyPod::cpyCtorCnt);
  EXPECT_EQ(0, NoisyPod::cpyAssignCnt);
}

TEST_F(CopyElision, LambdaBehavior) {
  auto aLambda = []() { return f3(); };
  // without calling aLambda, nothing has been created
  EXPECT_EQ(0, NoisyPod::dftCtorCnt);
  // after calling, the object is constructed by f3() and returned
  aLambda();
  EXPECT_EQ(1, NoisyPod::dftCtorCnt);

  // if you capture it, it should have been constructed without lambda called
  auto aLambda2 = [lt = f3()]() { return lt; };
  EXPECT_EQ(2, NoisyPod::dftCtorCnt);
  // calling it won't create a new one
  aLambda2();
  EXPECT_EQ(2, NoisyPod::dftCtorCnt);
}
*/
}  // namespace copy_elision