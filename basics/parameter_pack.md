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
- If the names of two parameter packs appear in the same pattern, they are expanded simultaneously, and they must have the same length:

```cpp
template <typename...> struct Tuple {};

template <typename T1, typename T2> struct Pair {};

template <class... Args1> struct zip {
    template <class... Args2> struct with {
        typedef Tuple<Pair<Args1, Args2>...> type;
        // Pair<Args1, Args2>... is the pack expansion
        // Pair<Args1, Args2> is the pattern
        // Args1 abd Args2 appear in the same pattern,
        // they are expanded simultaneously as below
    };
};

typedef zip<short, int>::with<unsigned short, unsigned>::type T1;
// Pair<Args1, Args2>... expands to
// Pair<short, unsigned short>, Pair<int, unsigned int>
// T1 is Tuple<Pair<short, unsigned short>, Pair<int, unsigned>>

typedef zip<short>::with<unsigned short, unsigned>::type T2;
// error: pack expansion contains parameter packs of different lengths
```
- If a pack expansion is nested within another pack expansion, the parameter packs that appear inside the innermost pack expansion are expanded by it, and there must be another pack mentioned in the enclosing pack expansion, but not in the innermost one:
```cpp
template <class... Args>
void g(Args... args) {
    f(const_cast<const Args*>(&args)...);
    // const_cast<const Args*>(&args) is the pattern, it expands two packs
    // (Args and args) simultaneously

    f(h(args...) + args...); // Nested pack expansion:
    // inner pack expansion is "args...", it is expanded first
    // outer pack expansion is h(E1, E2, E3) + args..., it is expanded
    // second (as h(E1, E2, E3) + E1, h(E1, E2, E3) + E2, h(E1, E2, E3) + E3)
}
```

* Depending on where the expansion takes place, the resulting comma-separated list is a different kind of list: **function parameter list, member initializer list, attribute list**, etc. The following is the list of all allowed contexts:

## Expansion as function argument lists
- A pack expansion may appear inside the parentheses of a function call operator, in which case the largest **expression** or **braced-init-list** to the **left** of the ellipsis `...` is the pattern that is expanded:
```cpp
f(&args...);             // expands to f(&E1, &E2, &E3)
f(n, ++args...);         // expands to f(n, ++E1, ++E2, ++E3);
f(++args..., n);         // expands to f(++E1, ++E2, ++E3, n);

f(const_cast<const Args*>(&args)...);
// f(const_cast<const E1*>(&X1), const_cast<const E2*>(&X2), const_cast<const E3*>(&X3))

f(h(args...) + args...); // expands to
// f(h(E1, E2, E3) + E1, h(E1, E2, E3) + E2, h(E1, E2, E3) + E3)
```
## Expansion as parenthesized initializers
- A pack expansion may appear inside the parentheses of a direct initializer, a function-style cast, and other contexts (member initializer, new-expression, etc.) in which case the rules are identical to the rules for a function call expression above:

```cpp
Class c1(&args...);             // calls Class::Class(&E1, &E2, &E3)
Class c2 = Class(n, ++args...); // calls Class::Class(n, ++E1, ++E2, ++E3);

::new((void *)p) U(std::forward<Args>(args)...) // std::allocator::allocate
```
## Expansion as brace-enclosed initializers
```cpp
template<typename... Ts>
void func(Ts... args)
{
    const int size = sizeof...(args) + 2;
    int res[size] = {1, args..., 2};

    // since initializer lists guarantee sequencing, this can be used to
    // call a function on each element of a pack, in order:
    int dummy[sizeof...(Ts)] = {(std::cout << args, 0)...};
}
```

## Expansion as template argument lists
- Pack expansions can be used anywhere in a template argument list, provided the template has the parameters to match the expansion:
```cpp
template<class A, class B, class... C>
void func(A arg1, B arg2, C... arg3)
{
    container<A, B, C...> t1; // expands to container<A, B, E1, E2, E3>
    container<C..., A, B> t2; // expands to container<E1, E2, E3, A, B>
    container<A, C..., B> t3; // expands to container<A, E1, E2, E3, B>
}
```

## Expansion as function parameter list
- In a function parameter list, if an ellipsis appears in a parameter declaration (whether it names a function parameter pack (as in, Args... args) or not) the parameter declaration is the pattern:
```cpp
template<typename... Ts>
void f(Ts...) {}

f('a', 1); // Ts... expands to void f(char, int)
f(0.1);    // Ts... expands to void f(double)

template<typename... Ts, int... N>
void g(Ts (&...arr)[N]) {}

// Note: In the pattern Ts (&...arr)[N], the ellipsis is the innermost element,
// not the last element as in all other pack expansions.

int n[1];
g<const char, int>("a", n); // Ts (&...arr)[N] expands to
                            // const char (&)[2], int(&)[1]
```

## Expansion as template parameter list
```cpp
template<typename... T>
struct value_holder
{
    template<T... Values> // expands to a non-type template parameter
    struct apply {};      // list, such as <int, char, int(&)[5]>
};
```

## Expansion as base specifiers and member initializer lists
```cpp
template<class... Mixins>
class X : public Mixins...
{
public:
    X(const Mixins&... mixins) : Mixins(mixins)... {}
};
```
## Expansion as lambda captures
```cpp
template<class... Args>
void f(Args... args)
{
    auto lm = [&, args...] { return g(args...); };
    lm();
}
```

## Expansion in the `sizeof...` operator
```cpp
template<class... Types>
struct count
{
    static const std::size_t value = sizeof...(Types);
};
```

## Expansion in dynamic exception specifications
```cpp
template<class... X>
void func(int arg) throw(X...)
{
    // ... throw different Xs in different situations
}
```

## Expansion in using-declarations
```cpp
template<typename... bases>
struct X : bases...
{
    using bases::g...;
};
X<B, D> x; // OK: B::g and D::g introduced
```

## Expansion in fold expressions
- check [fold expression](fold_expression.md)