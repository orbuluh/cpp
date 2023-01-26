#pragma once

#include <cstdio>

struct NoisyPod {
  NoisyPod() noexcept { puts("dft_ctor"); dftCtorCnt++; }
  ~NoisyPod() noexcept { puts("dtor"); }

  NoisyPod(const NoisyPod&) noexcept { puts("cpy_ctor"); cpyCtorCnt++; }
  NoisyPod(NoisyPod&&) noexcept { puts("move_ctor"); moveCtorCnt++; }

  NoisyPod& operator=(const NoisyPod&) noexcept {
    puts("cpy_assign");
    cpyAssignCnt++;
    return *this;
  }
  NoisyPod& operator=(NoisyPod&&) noexcept {
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

  static std::size_t dftCtorCnt;
  static std::size_t cpyCtorCnt;
  static std::size_t moveCtorCnt;
  static std::size_t cpyAssignCnt;
  static std::size_t moveAssignCnt;
};

size_t NoisyPod::dftCtorCnt = 0;
size_t NoisyPod::cpyCtorCnt = 0;
size_t NoisyPod::moveCtorCnt = 0;
size_t NoisyPod::cpyAssignCnt = 0;
size_t NoisyPod::moveAssignCnt = 0;