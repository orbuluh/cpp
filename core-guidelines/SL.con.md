# SL.con: Containers
- [SL.con: Containers](#slcon-containers)
  - [SL.con.1: Prefer using STL array or `vector` instead of a C array](#slcon1-prefer-using-stl-array-or-vector-instead-of-a-c-array)
  - [SL.con.2: Prefer using STL vector by default unless you have a reason to use a different container](#slcon2-prefer-using-stl-vector-by-default-unless-you-have-a-reason-to-use-a-different-container)
  - [SL.con.3: Avoid bounds errors](#slcon3-avoid-bounds-errors)
  - [SL.con.4: don't use `memset` or `memcpy` for arguments that are **not trivially-copyable**](#slcon4-dont-use-memset-or-memcpy-for-arguments-that-are-not-trivially-copyable)


## SL.con.1: Prefer using STL array or `vector` instead of a C array
- C arrays are less safe, and have no advantages over array and vector. For a fixed-length array, use `std::array`, which does not degenerate to a pointer when passed to a function and does know its size.
- Also, like a built-in array, a stack-allocated `std::array` keeps its elements on the stack.
- For a variable-length array, use `std::vector`, which additionally can change its size and handles memory allocation.
- Note:  Comparing the performance of a fixed-sized array allocated on the stack against a vector with its elements on the free store is bogus.
- You could just as well compare a `std::array` on the stack against the result of a `malloc()` accessed through a pointer.


## SL.con.2: Prefer using STL vector by default unless you have a reason to use a different container
- `vector` and `array` are the only standard containers that offer the following advantages:
  - the fastest general-purpose access (random access, including being vectorization-friendly);
  - the fastest default access pattern (begin-to-end or end-to-begin is prefetcher-friendly);
  - the lowest space overhead (contiguous layout has zero per-element overhead, which is cache-friendly).
- Usually you need to add and remove elements from the container, so use `vector` by default; if you don't need to modify the container's size, use `array`.
- Even when other containers seem more suited, such as map for O(log N) lookup performance or a list for efficient insertion in the middle, a `vector` will usually still perform better for containers up to a few KB in size.
- Note: `string` should not be used as a container of individual characters. A `string` is a textual string; if you want a container of characters, use `vector</*char_type*/>` or `array</*char_type*/>` instead.
- Exceptions: If you have a good reason to use another container, use that instead. For example:
  - If vector suits your needs but you don't need the container to be variable size, use array instead.
  - If you want a dictionary-style lookup container that guarantees O(K) or O(log N) lookups, the container will be larger (more than a few KB) and you perform frequent inserts so that the overhead of maintaining a sorted `vector` is infeasible, go ahead and use an `unordered_map` or `map` instead.
- Note:
  - To initialize a `vector` with a number of elements, use ()-initialization.
  - To initialize a vector with a list of elements, use {}-initialization.
```cpp
vector<int> v1(20);  // v1 has 20 elements with the value 0 (vector<int>{})
vector<int> v2 {20}; // v2 has 1 element with the value 20 Prefer the {}-initializer syntax.
```

## SL.con.3: Avoid bounds errors
- Read or write beyond an allocated range of elements typically leads to bad errors, wrong results, crashes, and security violations.
- Note: The standard-library functions that apply to ranges of elements all have (or could have) bounds-safe overloads that take span.
- Standard types such as `vector` can be modified to perform bounds-checks under the bounds profile (in a compatible way, such as by adding contracts), or used with `at(`).
- Ideally, the in-bounds guarantee should be statically enforced. For example:
  - a range-for cannot loop beyond the range of the container to which it is applied
  - a v.begin(),v.end() is easily determined to be bounds safe
  - Such loops are as fast as any unchecked/unsafe equivalent.
- Often a simple pre-check can eliminate the need for checking of individual indices. For example
  - for `v.begin()`,`v.begin()+i` the i can easily be checked against `v.size()`
  - Such loops can be much faster than individually checked element accesses.

```cpp
// Example, bad
void f() {
    array<int, 10> a, b;
    memset(a.data(), 0,
           10); // BAD, and contains a length error (length = 10 * sizeof(int))
    memcmp(a.data(), b.data(),
           10); // BAD, and contains a length error (length = 10 * sizeof(int))
}
// Also, std::array<>::fill() or std::fill() or even an empty initializer are
// better candidates than memset(). Example, good
void f() {
    array<int, 10> a, b, c{}; // c is initialized to zero
    a.fill(0);
    fill(b.begin(), b.end(), 0); // std::fill()
    fill(b, 0);                  // std::ranges::fill()

    if (a == b) {
        // ...
    }
}
```
- Example: If code is using an unmodified standard library, then there are still workarounds that enable use of `std::array` and `std::vector` in a bounds-safe manner.
- Code can call the `.at()` member function on each class, which will result in an `std::out_of_range` exception being thrown.
- Alternatively, code can call the `at()` free function, which will result in fail-fast (or a customized action) on a bounds violation.

```cpp
void f(std::vector<int>& v, std::array<int, 12> a, int i)
{
    v[0] = a[0];        // BAD
    v.at(0) = a[0];     // OK (alternative 1)
    at(v, 0) = a[0];    // OK (alternative 2)

    v.at(0) = a[i];     // BAD
    v.at(0) = a.at(i);  // OK (alternative 1)
    v.at(0) = at(a, i); // OK (alternative 2)
}
```

## SL.con.4: don't use `memset` or `memcpy` for arguments that are **not trivially-copyable**
- Doing so messes the semantics of the objects (e.g., by overwriting a `vptr`).
- Note: Similarly for `(w)memset`, `(w)memcpy`, `(w)memmove`, and `(w)memcmp`

```cpp
// Example
struct base {
    virtual void update() = 0;
};

struct derived : public base {
    void update() override {}
};

void f(derived& a, derived& b) // goodbye v-tables
{
    memset(&a, 0, sizeof(derived));
    memcpy(&a, &b, sizeof(derived));
    memcmp(&a, &b, sizeof(derived));
}
```
- Instead, define proper default initialization, copy, and comparison functions
```cpp
void g(derived& a, derived& b)
{
    a = {};    // default initialize
    b = a;     // copy
    if (a == b) do_something(a, b);
}
```
