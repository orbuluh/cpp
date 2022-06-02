# Notes from [C++ High Performance](https://www.amazon.com/High-Performance-Master-optimizing-functioning/dp/1839216549)
- When using metaprogramming, it is important to remember that its main use case is **to make great libraries and, thereby, hide complex constructs/optimizations from the user code.**
  - However complex the interior of the code for the metaprogram may be, it's important to hide it behind a good interface so that the user codebase is easy to read and use.
- Metaprogramming is a technique where we write code that transforms or generates some other code.
  - We could use any language to produce regular code, with C++, we have the privilege of writing metaprograms within the language itself using templates and constant expressions.
  - In its simplest and most common form, template metaprogramming in C++ is used to generate functions, values, and classes that accept different types.

- it's useful to think about C++ metaprogramming being carried out in these two distinct phases:
  - An initial phase, where templates and constant expressions produce regular C++ code of functions, classes, and constant values. This phase is usually called constant evaluation.
  - A second phase, where the compiler eventually compiles the regular C++ code into machine code.
- 