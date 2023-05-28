# Parameter pack

- reading notes from
  - [MODERNES C++](https://www.modernescpp.com/index.php/from-variadic-templates-to-fold-expressions)
  - [cppreference](https://en.cppreference.com/w/cpp/language/parameter_pack)

## Quick fact

- Why sometimes `...` is on left of `Args` and other time on right?
  - The ellipsis ... has different meanings depending on its position relative to a type or a parameter name. It provides a mechanism for working with variadic templates and variadic function arguments.
  - A **template parameter pack** `template<typename... T>` is a **template parameter** that accepts zero or more template arguments (non-types, types, or templates).
  - A **function parameter pack** `void f(T...)` is a **function parameter** that accepts zero or more function arguments.

```cpp
template<typename... Types> // A type template parameter pack with an optional name "Types"
void f(Types... args);  // A function parameter pack with an optional name "Types"


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

## Pack expansion

- The "unpacking" syntax is called **pack expansion** in official term, with a form of `pattern...` where `pattern` must include at least one parameter pack.
  - Pack expansion allows you to expand a parameter pack (a variadic template or function parameter pack) into a sequence of separate elements. It is denoted by the ellipsis `...` and can be used in several contexts to generate multiple elements from a pack.

- A pattern followed by an ellipsis: `pattern ...`, in which **the name of at least one parameter pack appears at least once,** is **expanded into zero or more comma-separated instantiations of the pattern,** where the **name of the parameter pack is replaced by each of the elements from the pack, in order.**

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

using T1 = zip<short, int>::with<unsigned short, unsigned>::type;
// Pair<Args1, Args2>... expands to
// Pair<short, unsigned short>, Pair<int, unsigned int>
// T1 is Tuple<Pair<short, unsigned short>, Pair<int, unsigned>>

using T2 = zip<short>::with<unsigned short, unsigned>::type;
// error: pack expansion contains parameter packs of different lengths
// e.g. zip<short> -> 1 param, with<unsigned short, unsigned> -> 2 params
```

- If a pack expansion is nested within another pack expansion, the parameter packs that appear inside the innermost pack expansion are expanded by it, and there must be another pack mentioned in the enclosing pack expansion, but not in the innermost one:

- When a pack expansion is nested within another pack expansion, it means that one pack is expanded inside another pack. This allows for more complex patterns and transformations involving multiple parameter packs.


```cpp
template <class... Args> // say Args is E1, E2, E3
void g(Args... args) {
    f(const_cast<const Args*>(&args)...);
    // const_cast<const Args*>(&args) is the pattern, it expands two packs
    // (Args and args) simultaneously - so it becomes
    //f(const_cast<const E1*>(&e1),
    //  const_cast<const E2*>(&e2),
    //  const_cast<const E3*>(&e3))

    f(h(args...) + args...); // Nested pack expansion:
    // inner pack expansion is "args...", it is expanded first
    // outer pack expansion is h(E1, E2, E3) + args..., it is expanded
    // as (h(E1, E2, E3) + E1, h(E1, E2, E3) + E2, h(E1, E2, E3) + E3)
}
```

- `const_cast<const Args*>(&args)...` is a pack expansion that occurs inside the function `g()`.
  - The `const_cast<const Args*>(&args)` expression is expanded for each element in the `Args` pack, resulting in a series of const pointers to `args`.

- In `f(h(args...) + args...);`, we have a nested pack expansion.
  - The inner pack expansion is `args...`, which expands the pack `args` by itself.
  - The outer pack expansion is `h(args...) + args...`, which involves both `h()` and `args`.
  - The inner pack expansion is expanded first, generating a series of args values.
  - Then, the outer pack expansion is expanded using those `args` values, resulting in a series of expressions:
  - `h(E1, E2, E3) + E1, h(E1, E2, E3) + E2, h(E1, E2, E3) + E3`, where E1, E2, and E3 represent the expanded args values.


- Depending on where the expansion takes place, the resulting comma-separated list is a different kind of list: **function parameter list, member initializer list, attribute list**, etc. The following is the list of all allowed contexts:

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
    //  computes the number of arguments in the parameter pack args.
    const int size = sizeof...(args) + 2;

    // initializes the array res using an initializer list.
    // The elements of the initializer list are 1, followed by the expanded
    // args..., and finally 2
    int res[size] = {1, args..., 2};

    // uses an initializer list to call a function on each element of the pack
    // args and initialize the array dummy.
    // Here, the expression (std::cout << args, 0) is evaluated for each args
    // element.

    // The comma operator has a special behavior in C++ where it evaluates
    // multiple expressions and returns the result of the last expression. In
    // this case, the comma operator is used to achieve two things in a single
    // expression:
    //      - Printing the value of args to std::cout
    //      - Providing a value of 0 for initialization
    // By combining these two expressions using the comma operator, the code
    // achieves the effect of printing each element of the parameter pack args
    // to std::cout while simultaneously initializing the array dummy with 0
    // values.

    // Note: since initializer lists guarantee sequencing, this can be used to
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

- In a function parameter list, if an ellipsis appears in a parameter declaration (whether it names a function parameter pack (as in, `Args... args`) or not) the parameter declaration is the pattern:

```cpp
template<typename... Ts>
void f(Ts...) {}

f('a', 1); // Ts... expands to void f(char, int)
f(0.1);    // Ts... expands to void f(double)

template<typename... Ts, int... N>
void g(Ts (&...arr)[N]) {}

// Ts (&...arr)[N]: This function parameter represents a parameter pack of
// references to arrays. Each reference arr corresponds to an array of type Ts
// with size N.
// The &... syntax is a pack expansion that expands the references for each
// element in the parameter pack.
//
// Note: The ellipsis (...) in the pattern Ts (&...arr)[N] is placed before the
// identifier (arr), which is different from other pack expansions where the
// ellipsis is usually placed after the identifier. This syntax is used to
// expand the references for each array element individually.


int n[1];
g<const char, int>("a", n); // Ts (&...arr)[N] expands to
                            // const char (&)[2], int(&)[1]
// The type deduction for Ts and N in this case results in `const char` and `2`
// for the first argument, and `int` and `1` for the second argument.
//
// So, the function template g is instantiated as g<const char, int>("a", n),
// and the function parameter arr expands to
// const char(&)[2] and int(&)[1] respectively.
//
// const char(&)[2] is a reference to a fixed-size array of 2 elements, and each
// element is of type const char. It is often used when you want to pass or
// manipulate an array as a whole, preserving its size and element type.
```

## Expansion as template parameter list

```cpp
template<typename... T>
struct value_holder
{
    //This is the template declaration inside value_holder. It introduces a
    // non-type template parameter pack Values of type T.
    template<T... Values>
    struct apply {}; // This defines the inner structure within value_holder.
};

// The purpose of this code is to provide a mechanism for creating an inner
// template apply with a non-type template parameter pack Values.

// The pack expansion T... in the template declaration allows you to specify a
// sequence of values of types T when instantiating the apply template.

// For example, you could instantiate it as

auto x = value_holder<int, char, int(&)[5]>::apply<42, 'a', arr>();

// (assuming arr is an array of suitable type) to create a specific instance of
// apply with the specified non-type template parameters.
```

## Expansion as base specifiers and member initializer lists

```cpp
template<class... Mixins>
        // The ... is a pack expansion that expands the pack Mixins into a
        // comma-separated list of base classes.
class X : public Mixins...
{
public:
    // Takes arguments of each Mixin type by reference. Inside the constructor's
    // initialization list, the pack expansion Mixins(mixins)... expands the
    // parameter pack mixins to initialize each base class with the
    // corresponding mixin object.
    X(const Mixins&... mixins)
        : Mixins(mixins)... {}
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
void func(int arg) throw (X...)
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