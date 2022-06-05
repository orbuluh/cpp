- reading notes from
  - [MODERNES C++](https://www.modernescpp.com/index.php/from-variadic-templates-to-fold-expressions)
  - [Fluent C++ post](https://www.fluentcpp.com/2021/03/12/cpp-fold-expressions/)

# Quick fact
- Since C++17
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
