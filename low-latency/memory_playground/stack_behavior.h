#pragma once

#include <iostream>
#include <memory>

namespace stack_behavior {

void func1(int* mainIntAddr) {
  auto i = 0;
  std::cout << "func1(): " << std::addressof(i)
            << " diffToMain=" << (mainIntAddr - std::addressof(i)) << '\n';
}
void func2(int* mainIntAddr) {
  auto i = 0;
  std::cout << "func2(): " << std::addressof(i)
            << " diffToMain=" << (mainIntAddr - std::addressof(i)) << '\n';
  func1(mainIntAddr);
}
void demo() {
  auto i = 0;
  std::cout << "main(): " << std::addressof(i) << '\n';
  func2(std::addressof(i));
  func1(std::addressof(i));
}

}  // namespace stack_behavior
