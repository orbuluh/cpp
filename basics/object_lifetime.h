#pragma once
#include <gtest/gtest.h>
#include <list>

namespace object_lifetime {

struct Lifetime {
  Lifetime() noexcept { puts("dft_ctor"); dftCtorCnt++; }
  ~Lifetime() noexcept { puts("dtor"); }

  Lifetime(const Lifetime&) noexcept { puts("cpy_ctor"); cpyCtorCnt++; }
  Lifetime(Lifetime&&) noexcept { puts("move_ctor"); moveCtorCnt++; }

  Lifetime& operator=(const Lifetime&) noexcept {
    puts("cpy_assign");
    cpyAssignCnt++;
    return *this;
  }
  Lifetime& operator=(Lifetime&&) noexcept {
    puts("move_assign");
    moveAssignCnt++;
    return *this;
  }

  static void resetCounter() {
    dftCtorCnt = 0;
    cpyCtorCnt = 0;
    moveCtorCnt = 0;
    cpyAssignCnt = 0;
    moveAssignCnt = 0;
  }

  static size_t dftCtorCnt;
  static size_t cpyCtorCnt;
  static size_t moveCtorCnt;
  static size_t cpyAssignCnt;
  static size_t moveAssignCnt;
};

size_t Lifetime::dftCtorCnt = 0;
size_t Lifetime::cpyCtorCnt = 0;
size_t Lifetime::moveCtorCnt = 0;
size_t Lifetime::cpyAssignCnt = 0;
size_t Lifetime::moveAssignCnt = 0;

Lifetime f1() { return Lifetime{}; }
Lifetime f2() { return f1(); }
Lifetime f3() { return f2(); }


class LifeTimeTest : public testing::Test {
  void SetUp() override {
    Lifetime::resetCounter();
  }
  void TearDown() override {
    std::cout << "---end of scope---\n";
  }
};

TEST_F(LifeTimeTest, NormalAssign) {
  auto lt = Lifetime{};
  EXPECT_EQ(1, Lifetime::dftCtorCnt);
  EXPECT_EQ(0, Lifetime::cpyCtorCnt);
  EXPECT_EQ(0, Lifetime::cpyAssignCnt);
}

TEST_F(LifeTimeTest, ReturnValueOptimization) {
  auto lt = f3();
  EXPECT_EQ(1, Lifetime::dftCtorCnt);
  EXPECT_EQ(0, Lifetime::cpyCtorCnt);
  EXPECT_EQ(0, Lifetime::cpyAssignCnt);
}

TEST_F(LifeTimeTest, LambdaBehavior) {
  auto aLambda = []() { return f3(); };
  // without calling aLambda, nothing has been created
  EXPECT_EQ(0, Lifetime::dftCtorCnt);
  // after calling, the object is constructed by f3() and returned
  aLambda();
  EXPECT_EQ(1, Lifetime::dftCtorCnt);

  // if you capture it, it should have been constructed without lambda called
  auto aLambda2 = [lt = f3()]() { return lt; };
  EXPECT_EQ(2, Lifetime::dftCtorCnt);
  // calling it won't create a new one
  aLambda2();
  EXPECT_EQ(2, Lifetime::dftCtorCnt);
}


} // namespace object_lifetime