# Template meta programming

- Notes from [C++ High Performance](https://www.amazon.com/High-Performance-Master-optimizing-functioning/dp/1839216549)


:bulb: When using metaprogramming, it is important to remember that its main use case is **to make great libraries and, thereby, hide complex constructs/optimizations from the user code. However complex the interior of the code for the metaprogram may be, it's important to hide it behind a good interface so that the user codebase is easy to read and use**.

## What is metaprogramming

:man_teacher: Metaprogramming is a technique where we **write code that transforms or generates some other code.**

When writing regular C++ code, it is eventually transformed into machine code.

- Metaprogramming, on the other hand, allows us to **write code that transforms itself into regular C++ code.**
- With the introduction of `constexpr` , `if constexpr` , and recently, C++ `concepts`, metaprogramming has become much more similar to writing regular code.
- In its simplest and most common form, template metaprogramming in C++ is used to **generate functions, values, and classes that accept different types.*


## Why metaprogramming with C++

There is nothing that stops us from generating C++ code by using other languages.

- We could, for example, do metaprogramming by using preprocessor macros extensively or writing a Python script that generates or modifies C++ files for us.
- Even though we could use any language to produce regular code, **with C++, we have the privilege of writing metaprograms within the language itself** using **templates** and **constant expressions**.
- The C++ compiler can execute our metaprogram and generate regular C++ code that the compiler will further transform into machine code.

Advantages of using C++ for metaprogramming

- We don't have to parse the C++ code (the compiler does that for us).
- There is excellent support for analyzing and manipulating C++ types when using C++ template metaprogramming.


## How to do metaproramming in C++

Doing metaprogramming directly within C++ using templates and constant expressions:

- A **template** is said to be instantiated when the compiler uses that template to generate a class or a function. The template code generates non-templated C++ code, which is then executed as regular code. If the generated C++ code does not compile, the error will be caught at compile time.
- **Constant expressions** are evaluated by the compiler to generate constant values:

## Mechanics

(Useful to think about) C++ metaprogramming being carried out in these two distinct phases:

- An **initial phase**, where templates and constant expressions produce regular C++ code of functions, classes, and constant values. This phase is usually called **constant evaluation**.
- A second phase, where the **compiler eventually compiles the regular C++ code into machine code**.

- The template code generates non-templated C++ code, which is then executed as regular code.
- If the generated C++ code does not compile, the error will be caught at compile time.

## Specializations of a template

It's also possible to provide a custom implementation for certain values of the template parameters.

- For function templates, we need to fix all template parameters when writing a specialization.
- For class templates, it is possible to specify only a subset of the template parameters. This is called **partial template specialization.**

The reason we **cannot** apply partial template specialization to functions is that **functions can be overloaded (and classes cannot)**.

- If we were allowed to mix overloads and partial specialization, it would be very hard to comprehend.

## Abbreviated function templates (C++20)

- A new abbreviated syntax for writing function templates by adopting the same style used by generic lambdas: Using `auto` for function parameter types, we are actually creating a function template rather than a regular function.

```cpp
/* Original function template */
template <typename T>
auto pow_n(const T& v, int n) {
   auto product = T{1};
   for (int i = 0; i < n; ++i) { product *= v; }
   return product;
}

/* Abbreviated function templates */
auto pow_n(const auto& v, int n) { // Declares a function template
   std::remove_cvref_t<decltype(v)> product{1}; // Instead of T{1}
   for (int i = 0; i < n; ++i) { product *= v; }
   return product;
}

/* we need an explicit placeholder for a type but none are available due to the
use of auto, so we need to use decltype here. Also, we can't just use
decltype(v), as above what have resulted to type: const T&, so we need extra
std::remove_cvref_t here! std::remove_cvref_t is introduced in C++20, before
that, you can use std::decay */
```

## std metafunctions/`<type_traits>`

When doing template metaprogramming, you may often find yourself in situations where you need information about the types you are dealing with at compile time.

- **Type traits let us extract information about the types our templates are dealing with**.

## Two categories of type traits:

:one: Type traits that **return information about a type as a boolean or an integer value**. (`*_v)`

- returns true or false , depending on the input, and ends with `_v` (short for value).
- The `_v` postfix was added in C++17.
- If your library implementation does not provide `_v` postfixes for type traits, then you can use the older version, `std::is_floating_point<float>::value`. In other words, remove the `_v` extension and add `::value` at the end.

```cpp
auto same_type = std::is_same_v<uint8_t, unsigned char>;
auto is_float_or_double = std::is_floating_point_v<decltype(3.f)>;
```

:two: Type traits that **return a new type** (`*_t`).

- returns **a new type** and ends with `_t` (short for type).

```cpp
using value_type = std::remove_pointer_t<int*>; // -> int
using ptr_type = std::add_pointer_t<float>; // -> float*
```

## All type traits are evaluated at compile time

For example:

```cpp
template<typename T>
auto sign_func(T v) -> int {
   if (std::is_unsigned_v<T>) {
      return 1;
   }
   return v < 0 ? -1 : 1;
}

auto sign = sign_func(uint32_t{42});

/* generated template instantiation */
int sign_func(uint32_t v) {
   if (true) {
      return 1;
   }
   return v < 0 ? -1 : 1;
}
```

===========tempmorary @ section: Programming with constant expressions 305/619===========