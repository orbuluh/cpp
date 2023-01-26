#pragma once

#include "util/noisy_pod.h"
#include <gtest/gtest.h>
#include <list>

namespace object_lifetime {

NoisyPod f1() { return NoisyPod{}; }
NoisyPod f2() { return f1(); }
NoisyPod f3() { return f2(); }

class LifeTimeTest : public testing::Test {
  void SetUp() override { NoisyPod::resetCounter(); }
  void TearDown() override { std::cout << "---end of scope---\n"; }
};

TEST_F(LifeTimeTest, NormalAssign) {
  auto lt = NoisyPod{};
  EXPECT_EQ(1, NoisyPod::dftCtorCnt);
  EXPECT_EQ(0, NoisyPod::cpyCtorCnt);
  EXPECT_EQ(0, NoisyPod::cpyAssignCnt);
}

TEST_F(LifeTimeTest, ReturnValueOptimization) {
  auto lt = f3();
  EXPECT_EQ(1, NoisyPod::dftCtorCnt);
  EXPECT_EQ(0, NoisyPod::cpyCtorCnt);
  EXPECT_EQ(0, NoisyPod::cpyAssignCnt);
}

TEST_F(LifeTimeTest, LambdaBehavior) {
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

}  // namespace object_lifetime