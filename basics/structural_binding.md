- Reading notes from
  - [Jean Guegant Blog](https://jguegant.github.io/blogs/tech/structured-bindings.html#structured-bindings)
- [Code playground](structural_binding.h)
# Structural binding

## Quick fact
- "structured binding" feature can be seen a syntax-sugar. **This means that we can produce a code equivalent to what a structured binding declaration would do.**
```cpp
auto [x, y] = foo();
```
- Under the hood:
  - I am declaring a new anonymous variable that encompass `[x, y]`.
  - This anonymous variable will have its type **deduced** and be a **copy** of what `foo` returns since we have `auto`.
```cpp
// e.g. under the hood
auto a_secret_variable = foo();
```
- What this implies is
  - **the `auto` keyword is applied to the anonymous variable**
  - **the `auto` keyword is NOT applied to the identifier `x` and `y`**
- Anonymous variable itself is not visible to anyone **except the compiler**, it is not possible to actually check its type.


- Now let's assume that `foo` is returning a type by value - `T foo()` - and therefore produce a temporary variable.

- This means that we can only use `auto`, `const auto&` or `auto&&` but **NOT `auto&`**.
  - `auto` will do a copy of that temporary,
  - `const auto&` and `auto&&` will bind to that temporary and prolong its life until the end of the scope.
  - `auto&` would NOT work here as l-value references cannot bind to temporary values.
- `auto&` will be useful only if your (member) function foo returns a l-value reference - `T& foo()`

- Note that the **expression on the right part** of the equal operator can be more complicated than just a call to a function like foo. Anything that could assign `a_secret_variable` could belong there, like `auto&& a_secret_variable = std::pair(2, "bob");`.

- Now that the compiler produced the anonymous variable, how do we obtain `x` and `y` from it?
  - This will depend on the type of expression on the right.
  - There are three cases: one for **array types**, one for **simple types** and one for **types that act like `std::tuple`**.

