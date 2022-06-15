- reading notes from
  - [MODERNES C++](https://www.modernescpp.com/index.php/from-variadic-templates-to-fold-expressions)
  - [Fluent C++ post](https://www.fluentcpp.com/2021/03/12/cpp-fold-expressions/)
- [playground](fold_expression.h)

# Quick fact
- Since C++17
- Reduces (folds) a [parameter pack](parameter_pack.md) over **a binary operator**.
- Four syntax (**Note that the opening and closing parentheses are a required part of the fold expression.**):
```cpp
( pack op ... )	// unary right fold
( ... op pack )	// unary left fold
( pack op ... op init )	// binary right fold
( init op ... op pack )	// binary left fold
```
  - 1) Unary right fold `(E op ...)` becomes `(E1 op (... op (EN-1 op EN)))`
  - 2) Unary left fold `(... op E)` becomes `(((E1 op E2) op ...) op EN)`
  - 3) Binary right fold `(E op ... op I)` becomes `(E1 op (... op (EN−1 op (EN op I))))`
  - 4) Binary left fold `(I op ... op E)` becomes `((((I op E1) op E2) op ...) op EN)`
- For example:
```cpp
// Example of 2: ( ... op pack )
template<typename... Args>
bool all(Args... args) { return (... && args); }

bool b = all(true, true, true, false);
 // within all(), the unary left fold expands as
 //  return ((true && true) && true) && false;
 // b is false
```
```cpp
// Example of 4: ( init op ... op pack )
template<typename ...Args>
void printer(Args&&... args)
{
    (std::cout << ... << args) << '\n';
}

// My aha moment!! What if I do...
template<typename ...Args>
void printer2(Args&&... args)
{
    std::cout << (... << args) << '\n';
}
printer2(1, 2, 3, 4); // result: 512
// why? This becomes form 2: ( ... op pack )
// so it basically becomes...
// std::cout << (((1 << 2) << 3) << 4) << '\n';
// e.g. 4 * 8 * 16 = 512!!!!
```
- A fold expression is an instruction for the compiler to repeat the application of an operator over a **variadic template pack.**
- The position of ... specifies left or right associativity, but doesn't change the order of arguments.
  - The (left/right) associativity is on the same side as the dot dot dot.
  - For example:
```cpp
// left fold
/* e.g.
int minus(int const& value1, int const& value2, int const& value3)
{
    return ((value1 - value2) - value3);
}                                                                 */
template <typename... Values>
auto minus(Values const&... values) {
    return (... + values);
}

// right fold
/* e.g.
int minus(int const& value1, int const& value2, int const& value3)
{
    return (value1 - (value2 - value3));
}                                                                 */
template <typename... Values>
auto minus(Values const&... values) {
    return (values + ...);
}
```
