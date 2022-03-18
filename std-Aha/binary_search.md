# Quick fact
- `#incldue <algorithm>`
- Pay attention to different customized comparator requirement below.
- [examples](binarySearch.h)
# std::lower_bound: find the first large or equal than tgt
- First element in the range [first,last) which >= taget val
- If all the element in the range compare less than val, the function returns last (a.k.a. end).
- comp signature: `bool pred(const Type1 &a, const Type2 &b);`
    - binary predicate which returns ​true if the first argument is less than (i.e. is ordered before) the second.
    - The type Type1 must be such that an object of *type ForwardIt* can be dereferenced and then implicitly converted to *Type1*.
    - The type Type2 must be such that an object of *type T* can be implicitly converted to *Type2*.​
    - e.g. comp could be -  `[](const vec_value_t& v, target_val_t tgt_val) { return f(v) < tgt_val; }`
# std::upper_bound: find the first begin than tgt
- Returns an iterator pointing to the first element in the range [first,last) which compares greater than val.
- If no element in the range compares greater than val, the function returns last (a.k.a. end).
- omp signature: `bool pred(const Type1 &a, const Type2 &b);`
    - binary predicate which returns ​true if the first argument is less than (i.e. is ordered before) the second.
    - The type Type1 must be such that an object of *type T* can be implicitly converted to *Type1*.
    - The type Type2 must be such that an object of *type ForwardIt* can be dereferenced and then implicitly converted to *Type2.*
    - ^^ different from lower_bound!!!​
    - e.g. comp could be - `[](target_val_t tgt_val. const vec_value_t& v) { return tgt_val < f(v); }`

# std::binary_search
- Checks if an element equivalent to value appears within the range [first, last).
- comp signature: `bool pred(const Type1 &a, const Type2 &b);`
    - binary predicate which returns ​true if the first argument is less than (i.e. is ordered before) the second.
    - The types Type1 and Type2 must be such that an object of *type T* can be implicitly converted to *both Type1 and Type2*,
    - An object of type ForwardIt can be dereferenced and then implicitly converted to *both Type1 and Type2*.​
