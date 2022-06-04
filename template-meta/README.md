# Notes from [C++ High Performance](https://www.amazon.com/High-Performance-Master-optimizing-functioning/dp/1839216549)
- When using metaprogramming, it is important to remember that its main use case is **to make great libraries and, thereby, hide complex constructs/optimizations from the user code.**
  - However complex the interior of the code for the metaprogram may be, it's important to hide it behind a good interface so that the user codebase is easy to read and use.
- Metaprogramming is a technique where we write code that transforms or generates some other code.
  - We could use any language to produce regular code, with C++, we have the privilege of writing metaprograms within the language itself using templates and constant expressions.
  - In its simplest and most common form, template metaprogramming in C++ is used to generate functions, values, and classes that accept different types.

- it's useful to think about C++ metaprogramming being carried out in these two distinct phases:
  - An initial phase, where templates and constant expressions produce regular C++ code of functions, classes, and constant values. This phase is usually called constant evaluation.
  - A second phase, where the compiler eventually compiles the regular C++ code into machine code.

# Basics

## Mechanics
- The template code generates non-templated C++ code, which is then executed as regular code.
- If the generated C++ code does not compile, the error will be caught at compile time.

## Specializations of a template
- It's also possible to provide a custom implementation for certain values of the template parameters.
  - For function templates, we need to fix all template parameters when writing a specialization.
  - For class templates, it is possible to specify only a subset of the template parameters. This is called partial template specialization.
- The reason we cannot apply partial template specialization to functions is that functions can be overloaded (and classes cannot). 
  - If we were allowed to mix overloads and partial specialization, it would be very hard to comprehend.

## std metafunctions/type traits
- When doing template metaprogramming, you may often find yourself in situations where you need information about the types you are dealing with at compile time. Type traits let us extract information about the types our templates are dealing with.
- All type traits are evaluated at compile time.
- Two categories of type traits:
- Type traits that return information about a type as a boolean or an integer value. (`*_v)`
  - The first category returns true or false , depending on the input, and ends with _v (short for value).
  - The `_v` postfix was added in C++17. If your library implementation does not provide _v postfixes for type traits, then you can use the older version, `std::is_floating_point<float>::value` .
  - In other words, remove the `_v` extension and add `::value` at the end.
- Type traits that return a new type (`*.t`).
    - The second category of type traits returns **a new type** and ends with `_t` (short for type).

