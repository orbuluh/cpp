#include <iostream>

#include "some_lib/some_lib.h"

int main() {
  std::cout << "Hello, World!\n";
  std::cout << "Utility value: " << some_lib::call_util() << '\n';
  return 0;
}