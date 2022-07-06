# T.meta: Template metaprogramming (TMP)
- Templates provide a general mechanism for compile-time programming.
- Metaprogramming is programming where at least one input or one result is a type.
- Templates offer Turing-complete (modulo memory capacity) duck typing at compile time. The syntax and techniques needed are pretty horrendous.

- [T.meta: Template metaprogramming (TMP)](#tmeta-template-metaprogramming-tmp)
  - [T.120: Use template metaprogramming only when you really need to](#t120-use-template-metaprogramming-only-when-you-really-need-to)
  - [T.121: Use template metaprogramming primarily to emulate concepts](#t121-use-template-metaprogramming-primarily-to-emulate-concepts)
  - [T.122: Use templates (usually template aliases) to compute types at compile time](#t122-use-templates-usually-template-aliases-to-compute-types-at-compile-time)
  - [T.123: Use `constexpr` functions to compute values at compile time](#t123-use-constexpr-functions-to-compute-values-at-compile-time)
  - [T.124: Prefer to use standard-library TMP facilities](#t124-prefer-to-use-standard-library-tmp-facilities)
  - [T.125: If you need to go beyond the standard-library TMP facilities, use an existing library](#t125-if-you-need-to-go-beyond-the-standard-library-tmp-facilities-use-an-existing-library)

## T.120: Use template metaprogramming only when you really need to
- Template metaprogramming is hard to get right, slows down compilation, and is often very hard to maintain.
- However, there are real-world examples where template metaprogramming provides better performance than any alternative short of expert-level assembly code.
- Also, there are real-world examples where template metaprogramming expresses the fundamental ideas better than run-time code.
- For example, if you really need AST manipulation at compile time (e.g., for optional matrix operation folding) there might be no other way in C++.
- Example, bad: `enable_if`. Instead, use `concept`s. But see How to emulate concepts if you don't have language support.
- Alternative: If the result is a value, rather than a type, use a `constexpr` function.
- Note: If you feel the need to hide your template metaprogramming in macros, you have probably gone too far.

## T.121: Use template metaprogramming primarily to emulate concepts
- Where C++20 is not available, we need to emulate them using TMP.
- Use cases that require `concept`s (e.g. overloading based on `concept`s) are among the most common (and simple) uses of TMP.

```cpp
template <typename Iter>
/*requires*/ enable_if<random_access_iterator<Iter>, void>
advance(Iter p, int n) {
    p += n;
}

template <typename Iter>
/*requires*/ enable_if<forward_iterator<Iter>, void>
advance(Iter p, int n) {
    assert(n >= 0);
    while (n--)
        ++p;
}
```
- Note: Such code is much simpler using concepts:
```cpp
void advance(random_access_iterator auto p, int n) { p += n; }
void advance(forward_iterator auto p, int n) { assert(n >= 0); while (n--) ++p;}
```

## T.122: Use templates (usually template aliases) to compute types at compile time
- Template metaprogramming is the only directly supported and half-way principled way of generating types at compile time.
- Note: "Traits" techniques are mostly replaced by **template aliases to compute types** and **`constexpr` functions to compute values**.


## T.123: Use `constexpr` functions to compute values at compile time
- A function is the most obvious and conventional way of expressing the computation of a value.
- Often a `constexpr` function implies less compile-time overhead than alternatives.
- Note: "Traits" techniques are mostly replaced by template aliases to compute types and constexpr functions to compute values.

```cpp
template<typename T>
    // requires Number<T>
constexpr T pow(T v, int n)   // power/exponential
{
    T res = 1;
    while (n--) res *= v;
    return res;
}

constexpr auto f7 = pow(pi, 7);
```

## T.124: Prefer to use standard-library TMP facilities
- Facilities defined in the standard, such as `conditional`, `enable_if`, and `tuple`, are portable and can be assumed to be known.

## T.125: If you need to go beyond the standard-library TMP facilities, use an existing library
- Getting advanced TMP facilities is not easy and using a library makes you part of a (hopefully supportive) community.
- Write your own "advanced TMP support" only if you really have to.
