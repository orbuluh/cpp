#include <iostream>
#include <memory>
#include <optional>

#include "util/pod.h"
namespace no_const {

inline S return_ideal() { return S{}; }
inline const S return_const() { return S{}; }
inline const S return_from_const_local() {
  const S s{};
  return s;
}
inline S return_const_local_with_multiple_branch(bool option) {
  if (option) {
    const S s;  // bad use of const
    return s;
  } else {
    const S s2;  // bad use of const
    return s2;   // better would be to return S{} directly!
  }
}
inline std::optional<S> return_optional_from_const_local() {
  const S s{};
  return s;  // will use copy!
}
inline std::optional<S> return_optional_from_nonconst_local() {
  S s{};
  return s;  // will use move
}
inline S return_const_param(const S s) {
  // const param will make 1 copy
  // return will make another copy
  return s;
}
void demo() {
  {
    puts("showcase pod");
    S s;
    auto s2 = s;
    auto s3 = std::move(s);
  }
  {
    puts("return non-const");
    S s = return_ideal();
  }
  {
    puts("return const non-reference");
    // make no difference by clang 14.0.6, but could create extra
    // construct and copy in other setup
    S s = return_const();
  }
  {
    puts("return from const local");
    // make no difference by clang 14.0.6, but could create extra
    // construct and copy in other setup
    S s = return_from_const_local();
  }
  {
    puts("return optional from branch 1");
    // make no difference by clang 14.0.6, but could create extra
    // construct and copy in other setup
    S s = return_const_local_with_multiple_branch(0);
    puts("return optional from branch 2");
    S s2 = return_const_local_with_multiple_branch(1);
  }
  {
    puts("return optional from const local");
    auto s = return_optional_from_const_local();
  }
  {
    puts("return optional from non-const local");
    auto s = return_optional_from_nonconst_local();
  }
  {
    puts("return const parameter");
    S s;
    S s2 = return_const_param(s);
  }
}

}  // namespace no_const