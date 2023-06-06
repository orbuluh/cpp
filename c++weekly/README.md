# C++ weekly notes

## [Ep 322 - Top 4 Places To Never Use `const`](https://youtu.be/dGCxMmGvocE)

<details><summary markdown="span">Notes</summary>

- don't `const` non-reference return types (it breaks copy elision when return)
- don't `const` local values that need take advantage of implicit move-on-return operations

<details><summary markdown="span">Details</summary>

- [code snippet](demo/no_const.h)
- 
```cpp
inline std::optional<S> make_value_3() {
  const S s; // bad because this object and the return type
             // are different! The optional will make a copy from the local s
  return s;
}
```

- Note: if you have multiple different objects that might be returned, then you are also relying on implicit move-on-return (aka automatic move)

```cpp
inline S make_value_3_1(bool option) {
  if (option) {
    const S s; // bad use of const
    return s;
  } else {
    const S s2; // bad use of const
    return s2; // better would be to return S{} directly!
  }
}
```

- don't `const` non-trivial value parameters that you might need to return directly from the function
```cpp
inline S make_value_2(const S s) // return statement makes this bad use of `const`
{
  return s; // because we return it, const is bad in function definition!
}
```

- don't `const` *any* member data!
  - it breaks implicit and explicit moves
  - it breaks common use cases of assignment

</summary></details>


</summary></details>



## [Ep 339 - `static constexpr` vs `inline constexpr`](https://youtu.be/QVHwOOrSh3w)

<details><summary markdown="span">Notes</summary>

- `static constexpr` for `constexpr` values at function scope.
- `inline constexpr` for `constexpr` values at file scope.

<details><summary markdown="span">ChatGPT</summary>

`static constexpr` for `constexpr` values at function scope:

- When a `constexpr` value is declared within a function, it is typically used for local computations or as a constant local to that function.
- Since the variable is `constexpr`, it is a compile-time constant, and its value is computed at compile-time. Making it static ensures that the value is computed only once and shared among invocations of the function.
- Using `static` avoids unnecessary recomputation of the value on each function call and improves performance.
- In the context of `constexpr` values at function scope, the `inline` keyword is not needed because the value is already known at compile-time. The `constexpr` specifier ensures that the value is computed at compile-time, and since it is a constant, it can be accessed efficiently without the need for inlining.

`inline constexpr` for constexpr values at file scope:

- When a `constexpr` value is declared at file scope, it is typically intended to be used across multiple functions within the same translation unit.
- By using `inline constexpr`, it suggests that the variable's definition can be inlined by the compiler, potentially improving performance by eliminating function call overhead.
- The `inline` keyword allows the compiler to consider inlining the variable's value wherever it is used within the translation unit.
- Since **file scope variables are already implicitly `static`**, they have internal linkage, meaning they are not accessible from other translation units.


The statement "variables declared at file/global scope are implicitly static" can be rationalized by understanding the default storage duration and linkage of variables declared at file/global scope in C++.

- In C++, when a variable is declared at file/global scope (outside of any function or class), it has a default storage duration of "static."
- The "static" storage duration means that the variable exists for the entire duration of the program's execution.
- Additionally, variables declared at file/global scope also have "internal linkage" by default.
- Internal linkage means that the variable is only accessible within the translation unit where it is defined. Other translation units (source files) cannot access this variable directly.
- By implicitly making variables at file/global scope `static`, C++ ensures that these variables have a lifetime that spans the entire program execution and are limited to the scope of the translation unit. It helps prevent name collisions and provides encapsulation by default.

</summary></details>
</summary></details>
