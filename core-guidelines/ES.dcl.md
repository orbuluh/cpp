## ES.11: Use `auto` to avoid redundant repetition of type names

- Simple repetition is tedious and error-prone.
- When you use `auto`, the name of the declared entity is in a fixed position in the declaration, increasing readability.
- In a function template declaration the return type can be a member type.
```cpp
auto p = v.begin();      // vector<DataRecord>::iterator
auto z1 = v[3];          // makes copy of DataRecord
auto& z2 = v[3];         // avoids copy
const auto& z3 = v[3];   // const and avoids copy
auto h = t.future();
auto q = make_unique<int[]>(s);
auto f = [](int x) { return x + 10; };
```
- In each case, we save writing a longish, hard-to-remember type that the compiler already knows but a programmer could get wrong.

```cpp
template<class T>
auto Container<T>::first() -> Iterator;   // Container<T>::Iterator
```
- Exception: Avoid `auto` for initializer lists **and in cases where you know exactly which type you want and where an initializer might require conversion.**
```cpp
auto lst = { 1, 2, 3 };   // lst is an initializer list
auto x{1};   // x is an int (in C++17; initializer_list in C++11)
```
- Note, As of C++20, we can (and should) use concepts to be more specific about the type we are deducing:
```cpp
forward_iterator auto p = algo(x, y, z);
//Example (C++17)
std::set<int> values;
// ...
auto [ position, newly_inserted ] = values.insert(5);   // break out the members of the std::pair
```

## ES.28: Use lambdas for complex initialization, especially of const variables
- It nicely encapsulates local initialization, including cleaning up scratch variables needed only for the initialization, without needing to create a needless non-local yet non-reusable function.
- It also works for variables that should be const but only after some initialization work.
- If at all possible, reduce the conditions to a simple set of alternatives (e.g., an enum) and don't mix up selection and initialization.
```cpp
// Bad
widget x;                               // should be const, but:
for (auto i = 2; i <= N; ++i) {         // this could be some
    x += some_obj.do_something_with(i); // arbitrarily long code
} // needed to initialize x
// from here, x should be const, but we can't say so in code in this style
```
```cpp
// Prefer
const widget x = [&] {
    widget val; // assume that widget has a default constructor
    for (auto i = 2; i <= N; ++i) {           // this could be some
        val += some_obj.do_something_with(i); // arbitrarily long code
    }                                         // needed to initialize x
    return val;
}();
```