#include <iostream>

#include "template_lib.h"

int main() {
  std::cout << "Hello, World!\n";
  std::cout << "Utility value: " << template_lib::call_util() << '\n';
  return 0;
}