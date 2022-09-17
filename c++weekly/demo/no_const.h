#include <iostream>
#include <memory>

#include "util/pod.h"
namespace no_const {

inline S make_value() { return S{}; }
// bad, non-& const return type!
inline const S make_value_with_return_const() { return S{}; }

void demo() {
  {
    puts("make value with return non-const");
    S s = make_value();
  }
  {
    puts("make value with return const");
    S s2 = make_value_with_return_const();
  }
}



}  // namespace no_const