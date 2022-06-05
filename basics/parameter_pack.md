- reading notes from
  - [MODERNES C++](https://www.modernescpp.com/index.php/from-variadic-templates-to-fold-expressions)
  - [cppreference](https://en.cppreference.com/w/cpp/language/parameter_pack)

# Quick fact

- A **function** parameter pack is a function parameter that accepts zero or more function arguments.
```cpp
template<typename... Types> // A function parameter pack with an optional name "Types"
void f(Types... args);
```
- A **template** parameter pack is a template parameter that accepts zero or more template arguments (non-types, types, or templates).
```cpp
template<typename... Types> // A type template parameter pack with an optional name "Types"
struct Tuple {};

template<int... Values> // A non-type template parameter pack with an optional name "Types"
struct IntTuple {};
```
- A template with **at least one parameter** pack is called a **variadic template**.

- In a primary **class** template, the template parameter pack **must be the final parameter in the template parameter list**.
```cpp
template<typename U, typename... Ts>    // OK: can deduce U
struct valid;
// template<typename... Ts, typename U> // Error: Ts... not at the end
// struct Invalid;
```
- In a **function** template, the template parameter pack **may** appear earlier in the list provided that all following parameters can be deduced from the function arguments, or have default arguments
```cpp
template<typename... Ts, typename U, typename=void>
void valid(U, Ts...);    // OK: can deduce U
// void valid(Ts..., U); // Can't be used: Ts... is a non-deduced context in this position

valid(1.0, 1, 2, 3);     // OK: deduces U as double, Ts as {int, int, int}
```

# Pack expansion
- The "unpacking" syntax is called **pack expansion** in official term, with a form of `pattern...` where `pattern` must include at least one parameter pack.
- A pattern followed by an ellipsis, in which **the name of at least one parameter pack appears at least once,** is **expanded into zero or more comma-separated instantiations of the pattern,** where the **name of the parameter pack is replaced by each of the elements from the pack, in order.**
```cpp
template<typename... Us>
void foo(Us... pargs) {} // called by bar below

template<typename... Ts>
void bar(Ts... args)
{
    foo(&args...); // “&args...” is a pack expansion
                   // “&args” is its pattern
}

g(1, 0.2, "a"); // `Ts... args` expand to `int E1, double E2, const char* E3`
                // &args... expands to &int E1, &int E2, &int E3,
                // `Us... pargs` therefore expand to `int* E1, double* E2, const char** E3`
```



# Example
```cpp
bool allVar() { return true; } // (1) base case for recursion

template<typename T, typename ...Ts>  // (2) variadic template, with one parameter pack using packing syntax
bool allVar(T t, Ts ... ts) {  // (3) unpacking syntax
    return t && allVar(ts...); // (4) unpacking syntax
}
```