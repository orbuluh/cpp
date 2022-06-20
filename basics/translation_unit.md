# Translation unit

- Reading from [Internal and External Linkage in C++](http://www.goldsborough.me/c/c++/linker/2016/03/30/19-34-25-internal_and_external_linkage_in_c++/)

## Quick facts

- A *translation unit* refers to an implementation (`.c/.cpp`) file and all header (`.h/.hpp`) files it includes.
- If an object or function inside such a translation unit has [*internal linkage*](linkage.md), then that specific symbol is only visible to the linker within that translation unit.
  - The `static` keyword, when used in the global namespace, forces a symbol to have internal linkage.
- If an object or function has *external linkage*, the linker can also see it when processing other translation units.
  - The `extern` keyword results in a symbol having external linkage.
- The compiler defaults the linkage of symbols such that:
  - *Non-const* global variables have *external* linkage by default
  - *Const* global variables have *internal* linkage by default
  - Functions have *external* linkage by default

## Basics
- Definition of the term *symbol*
  - the term *symbol* to refer to **any kind of “code entity” that a linker works with**
  - i.e. *variables* and *functions* (and also `class`es/`struct` ...).

- The difference between *declaring* a symbol and *defining* a symbol
  - A *declaration* tells the compiler about the **existence** of a certain symbol and makes it **possible to refer to** that symbol everywhere where the **explicit memory address** or required storage of that symbol is not required.- Lets first cover two rudimentary concepts that we’ll need to properly discuss linkage:
  1. The difference between a declaration and a definition
  2. Translation Units
- A *definition* tells the compiler what the body of a function contains or **how much memory it must allocate for a variable**.
- Symbol may be *declared many times, but defined only once*.

- Scenarios when **a declaration is sufficient to the compiler**
  - It is always allowed to have pointers to a declared (but not defined) type, because pointers require fixed memory capacity (e.g. 8 bytes on 64-bit systems) and do not depend on the type pointed to.
  - For function declarations, **all parameters (no matter whether taken by value, reference or pointer) and the return type need only be declared** and not defined.

- Scenarios **requires a definition**
  - when a data member of a class is of reference or value (as in, neither reference nor pointer) type.
  - When you **dereference** that pointer, the definition does become necessary.
  - Definitions of parameter and return value types only **become necessary for the function definition.**


### Functions
- The difference between the declaration and definition of a function is fairly obvious:
```cpp
int f();               // declaration
int f() { return 42; } // definition
```

### Variables
- For variables, it is a bit different. Declaration and definition are usually not explicitly separate. 
- Most importantly, this does not just declare `x`, but also define it by calling the default constructor of `int`.
```cpp
int x;
```
  - (As an aside, in C++, as opposed to Java, the constructor of primitive types (such as `int`) does not default-initialize (in C++ lingo: *value initialize*) the variable to `0`. The value of `x` above will be whatever garbage lay at the memory address allocated for it by the compiler.)

- You can, however, explicitly separate the declaration of a variable from its definition by using the `extern` keyword:
```cpp
extern int x; // declaration
int x = 42;   // definition
```
- However, when `extern` is pre-pended to the declaration and an initialization is provided as well, then the expression turns into a definition and the `extern` keyword essentially becomes useless:

```cpp
extern int x = 5; // is the same thing as int x = 5;
```

### Forward Declaring
- What we mean by this is that we declare the type and name of a symbol so that we can use it **where its definition is not required.**
- By doing so, we don’t have to include the full definition of a symbol (usually a header file) when it is not explicitly necessary.
- This way, we reduce dependency on the file containing the definition.
- The main advantage of this is that **when the file containing the definition changes, the file where we forward declared that symbol does not need to be re-compiled** (and therefore, **also not all further files including it**).

- **Example**
- Say we have a function declaration (also called *prototype*) for `f`, taking an object of type `Foo` by value:

```cpp
// file.hpp
void f(Foo object);
```
- Now, the naïve thing to do would be to include `Foo`’s definition right away. But because we only declare `f` here, it is sufficient to provide the compiler with a declaration of `Foo`.
- This way, the compiler can identify the function by its prototype, but we can remove the dependency of `file.hpp` on the file containing the definition of `Foo`, say `foo.hpp`:

```cpp
// file.hpp
class Foo;
// no need to include foo.hpp

void f(Foo object);
```
- Now, say, we include `file.hpp` in 100 other files. And say we change `Foo`’s definition in `foo.hpp`.
- If we had included `foo.hpp` in `file.hpp`, `file.hpp` and all 100 files including it would have to be recompiled.
- By forward declaring `Foo`, the only files requiring recompilation are `foo.hpp` and `foo.cpp` (assuming that’s where `f` is defined).

# The ODR([one definition rule](https://en.wikipedia.org/wiki/One_Definition_Rule))
- One very important difference between declarations and definitions is that a symbol may be *declared many times, but defined only once*.
- For example, you can forward declare a function or class however often you want, but you may only ever have one definition for it.
- Therefore, this is valid C++:

```cpp
int f();
int f();
int f();
int f();
int f();
int f();
int f() { return 5; }
```

- While this isn’t:

```cpp
int f() { return 6; }
int f() { return 9; }
```

# Translation Units
- Programmers usually deal with header **files** and implementation **files**. Compilers don’t – they deal with ***translation units* (TUs)**, sometimes referred to as ***compilation units***.
- **The definition of such a translation unit is very simple: Any file, fed to the compiler, after it has been pre-processed*.**
- In detail, this means that **it is the file resulting from the pre-processor expanding macros,** conditionally including source code depending on `#ifdef` and `#ifndef` statements and copy-pasting any `#include`ed files.

```cpp
// header.hpp
#ifndef HEADER_HPP
#define HEADER_HPP

#define VALUE 5

#ifndef VALUE
struct Foo { private: int ryan; };
#endif

int strlen(const char* string);

#endif /* HEADER_HPP */
```

```cpp
//program.cpp
#include "header.hpp"

int strlen(const char* string)
{
	int length = 0;

	while(string[length]) ++length;

	return length + VALUE;
}
```

- The pre-processor will produce the following translation unit, which is then fed to the compiler:

```cpp
int strlen(const char* string);

int strlen(const char* string)
{
	int length = 0;

	while(string[length]) ++length;

	return length + 5;
}
```

# Linkage
- Now that we’ve covered the basics, we can deal with linkage.
- Linkage can be either *internal* or *external*.
- In general, linkage will refer to **the visibility of symbols to the linker when processing files.**
  - Do not confuse the term *visible* here with access rights like `private`. 
- ***Visibility*** here means that the **linker will only be able to use this symbol when processing the translation unit in which the symbol was declared,** and not later (as with symbols with `external` linkage).

## External Linkage

- When a symbol (variable or function) has external linkage, that means that **that symbol is visible to the linker from other files,** i.e. it is “**globally” visible and can be shared between translation units**.
- **In practice, this means that you must define such a symbol in a place where it will end up in one and only one translation unit,** typically an implementation file (`.c`/`.cpp`), such that **it has only one visible definition.**
- If you were to define such a symbol on the spot, along with declaring it, or to place its definition in the same file you declare it, you run the risk of making your linker very angry.
  - As soon as you include that file in more than one implementation file, such that its definition ends up in more than one translation unit, your linker will start crying.
- In C and C++, the `extern` keyword (explicitly) declares a symbol to have external linkage:

```cpp
// Both of these symbols have external linkage.
extern int x;
extern void f(const std::string& argument);
```

- Above it was mentioned that `const` global variables have *internal* linkage by default, and non-`const` global variables have *external* linkage by default.

- That means that `int x;` is the same as `extern int x;`, right?
  - Not quite. `int x;` is actually the same as `extern int x{};` 
  - (using C++11 uniform/brace initialization syntax to avoid the most vexing parse), as `int x;` not only declares, but also defines `x`.
  - Therefore, not prepending `extern`to `int x;`in the global scope is just as bad as also defining a variable when declaring it as `extern`:

```cpp
int x;          // is the same as
extern int x{}; // which will both likely cause linker errors.

extern int x;   // while this only declares the integer, which is ok.
```

- Example Badness
- Let’s declare a function `f` with external linkage in `file.hpp` and also define it in the same file:

```cpp
// file.hpp

#ifndef FILE_HPP
#define FILE_HPP

extern int f(int x);

/* ... */

int f(int) { return x + 1; }

/* ... */

#endif /* FILE_HPP */
```

- Note that pre-pending `extern` here is redundant, as **all functions are implicitly `extern`, and separating the declaration from the definition here is also unnecessary.** So let’s just quickly rewrite this as:

```cpp
// file.hpp

#ifndef FILE_HPP
#define FILE_HPP

int f(int) { return x + 1; }

#endif /* FILE_HPP */
```

- So let’s see why this is bad. We’ll now have two implementation files: `a.cpp` and `b.cpp`, both including this `file.hpp`:

```cpp
// a.cpp

#include "file.hpp"

/* ... */

```

```cpp
// b.cpp

#include "file.hpp"

/* ... */

```

- Now let the compiler do its job and generate two translation units for the two implementation files above (remember that `#include`ing means to literally copy-paste):

```cpp
// TU A, from a.cpp

int f(int) { return x + 1; }

/* ... */
```

```cpp
// TU B, from b.cpp

int f(int) { return x + 1; }

/* ... */
```

- At this point, the linker will step in (**linking comes after compilation**).
- The linker will pick up the symbol `f` and look for definitions. Linker will even find two definitions - One in TU A and one in TU B.
- The linker will stop and tell you in a way similar to this:

```shell
duplicate symbol __Z1fv in:
/path/to/a.o
/path/to/b.o
```

- The linker found two definitions for the same symbol `f`. Because it had external linkage, `f` was visible to the linker when processing both TU A and TU B. Naturally, this violates the ODR, so this causes a linker error.
- More specifically, this is when you get a *duplicate symbol* error, which is the one you’ll get most often along with an *undefined symbol* error (if we had only ever declared, but never defined `f`).

# Common usage for extern

- A common use case for declaring variables explicitly `extern` are **global variables.**
```cpp
// global.hpp

namespace Global {
extern unsigned int clock_rate;
}

// global.cpp
namespace Global {
unsigned int clock_rate = 1'000'000;
}
```

## Internal Linkage

- When a **symbol** has *internal linkage*
  - it will only be visible within the current translation unit.
  - In practice,  this means when you declare a symbol to have internal linkage in a header file, **each translation unit you include this file in will get *its own unique copy of that symbol*.**
  - I.e. it will be as if you redefined each such symbol in every translation unit.
- For **objects**, this means that the **compiler will literally allocate an entirely new, unique copy for each translation unit**, which can obviously incur high memory cost

- To declare a symbol with internal linkage
  - C and C++ provide the `static` keyword.
  - Its usage here is entirely separate from its usage in classes or functions (or, generally, any block).
- Here an example:

```cpp
// file1.hpp
void function1();
```

```cpp
// file2.hpp
void function2();
```

```cpp
// file1.cpp
#include "header.hpp"

void function1() { variable = 10; }
```

```cpp
// file2.cpp
#include "header.hpp"

void function2() { variable = 123; }
```

```cpp
// header.hpp
static int variable = 42;
```

```cpp
// main.cpp:
#include "header.hpp"
#include "file1.hpp"
#include "file2.hpp"

#include <iostream>

void main()
{
	function1();
	function2();

    // print 42, TU main.cpp's own copy from header.hpp, regardless the change
    // in function1, function2 (those have its own copy as well
	std::cout << variable << std::endl;
}
```

- Because `variable` has internal linkage, each translation unit that includes `header.hpp` gets its own unique copy of `variable`.
- Here, there are three translation units:
  - file1.cpp
  - file2.cpp
  - main.cpp
- When `function1` is called, `file1.cpp`’s copy of `variable` is set to 10.
- When `function2` is called, `file2.cpp`’s copy of `variable` is set to 123.
- However, the value printed out in `main.cpp` is `variable`, unchanged: 42.

- In C++, there exists another way to declare one or more symbols to have **internal** linkage: **anonymous namespaces**

- Such a namespace ensures that the s**ymbols declared inside it are visible only within the current translation unit.** It is, **in essence, just a way to declare many symbols as `static`.**
- In fact, for a while, **the `static` keyword for the use of declaring a symbol to have internal linkage was deprecated in favor of anonymous namespaces.**
- However, it was recently *undeprecated*, because it is useful to declare a single variable or function to have internal linkage.
- There are also a few minor differences which I won’t go into here.
- In any case, this:

```cpp
namespace { int variable = 0; }
```
- does (almost) the same thing as this:

```cpp
static int variable = 0;
```

## Internal linkage Usage
- When and why would one make use of internal linkage?
  - For objects, it is probably most often a very bad idea to make use of it. The memory cost can be very high for large objects given that each translation unit gets its own copy.
  - But mainly, it can really just cause odd, unexpected behavior. Imagine you had a singleton (a class of which you instantiate only a single instance), and would suddenly end up having multiple instances of your “singleton” (one for every translation unit).

- However, one interesting use case could be to **hide translation-unit-local helper functions from the global scope.**

- Imagine you have a helper function `foo` in your `file1.hpp` which you use in `file1.cpp`, but then you also have a helper function `foo` in your `file2.hpp` which you use in `file2.cpp`.
  - The first `foo` does something completely different than the second `foo`, but you cannot think of a better name for them.
  - So, you can declare them both `static`.
  - Unless you include both `file1.hpp` and `file2.hpp` in some same translation unit, this will hide the respective `foo`s from each other.
  - If you don’t declare them `static`, they will implicitly have external linkage and the first `foo`’s definition will collide with the second `foo`s definition and cause a linker error due to a violation of the one-definition-rule.
