/*From book https://www.amazon.com/High-Performance-Master-optimizing-functioning/dp/1839216549 Ch. 7*/
#pragma once

#include <iostream>
#include <memory>

size_t allocated = 0;

void* operator new(size_t size) {
  void* p = std::malloc(size);
  allocated += size;
  return p;
}

void operator delete(void* p) noexcept { std::free(p); }

namespace small_string_optimization {

void demo() {
  for (int sz : {0, 7, 15, 16, 20}) {
    allocated = 0;
    auto s = std::string("a", sz);
    // when heap is used, libc++ always stores a null-terminated string
    // internally and, therefore, needs an extra byte at the end for the null
    // character.
    std::cout << "string size = " << sz << " stack space = " << sizeof(s) << ", heap space = " << allocated
              << ", capacity = " << s.capacity() << '\n';
  }
}

}  // namespace small_string_optimization
