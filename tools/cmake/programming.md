# Programming in CMake

##　Control flow

There are a series of all caps keywords you can use inside an if statement, and you can often refer to variables by either directly by name or using the `${}` syntax (the if statement historically predates variable expansion)

```cmake
if(variable)
    # If variable is `ON`, `YES`, `TRUE`, `Y`, or non zero number
else()
    # If variable is `0`, `OFF`, `NO`, `FALSE`, `N`, `IGNORE`, `NOTFOUND`, `""`, or ends in `-NOTFOUND`
endif()
# If variable does not expand to one of the above, CMake will expand it then try again
```

Since this can be a little confusing if you explicitly put a variable expansion, like `${variable}`, due to the potential expansion of an expansion, a policy (CMP0054) was added in CMake 3.1+ that keeps a quoted expansion from being expanded yet again. So, as long as the minimum version of CMake is 3.1+, you can do:

```cmake
if("${variable}")
    # True if variable is not false-like
else()
    # Note that undefined variables would be `""` thus false
endif()
```

There are a variety of keywords as well, such as:

- **Unary**: `NOT`, `TARGET`, `EXISTS (file)`, `DEFINED`, etc.
- **Binary**: `STREQUAL`, `AND`, `OR`, `MATCHES` (regular expression), `VERSION_LESS`, `VERSION_LESS_EQUAL` (CMake 3.7+), etc.
- **Parenthese**s can be used to group


## generator-expressions

Most CMake commands happen at configure time, include the if statements seen above. But what if you need logic to occur at build time or even install time? Generator expressions were added for this purpose. **They are evaluated in target properties.**

- The simplest generator expressions are informational expressions, and are of the form `$<KEYWORD>`; they evaluate to a piece of information relevant for the current configuration.
- The other form is `$<KEYWORD:value>`, where `KEYWORD` is a keyword that controls the evaluation, and value is the item to evaluate (an informational expression keyword is allowed here, too).
- If `KEYWORD` is a generator expression or variable that evaluates to 0 or 1, value is substituted if 1 and not if 0.
- You can nest generator expressions, and you can use variables to make reading nested variables bearable.
- Some expressions allow multiple values, separated by commas.

If you want to put a compile flag only for the DEBUG configuration, for example, you could do:

`target_compile_options(MyTarget PRIVATE "$<$<CONFIG:Debug>:--my-flag>")`

- This is a newer, better way to add things than using specialized `*_DEBUG` variables, and generalized to all the things generator expressions support.
- Note that you should never, never use the configure time value for the current configuration, because multi-configuration generators like IDEs do not have a "current" configuration at configure time, only at build time through generator expressions and custom `*_<CONFIG>` variables.

Other common uses for generator expressions:

- **Limiting an item to a certain language only**, such as CXX, to avoid it mixing with something like CUDA, or wrapping it so that it is different depending on target language.
- **Accessing configuration dependent properties**, like target file location.
- **Giving a different location for build and install directories**.

That last one is very common. You'll see something like this in almost every package that supports installing:

```cmake
target_include_directories(
    MyTarget
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
```

## Macros and Functions

**The only difference between a function and a macro is **scope**; macros don't have one.**

- So, if you set a variable in a function and want it to be visible outside, you'll need `PARENT_SCOPE`.
- Nesting functions therefore is a bit tricky, since you'll have to explicitly set the variables you want visible to the outside world to `PARENT_SCOPE` in each function.
- But, functions don't "leak" all their variables like macros do.

An example of a simple function is as follows:

```cmake
function(SIMPLE REQUIRED_ARG)
    message(STATUS "Simple arguments: ${REQUIRED_ARG}, followed by ${ARGN}")
    set(${REQUIRED_ARG} "From SIMPLE" PARENT_SCOPE)
endfunction()
```

The output would be:

```bash
simple(This Foo Bar)
message("Output: ${This}")
-- Simple arguments: This, followed by Foo;Bar
Output: From SIMPLE
```

- If you want positional arguments, they are listed explicitly, and all other arguments are collected in ARGN (ARGV holds all arguments, even the ones you list)
-  You have to work around the fact that **CMake does not have return values by setting variables**. In the example above, you can explicitly give a variable name to set.

## Arguments

CMake has a named variable system that you've already seen in most of the build in CMake functions. You can use it with the `cmake_parse_arguments` function.
If you want to support a version of CMake less than 3.5, you'll want to also include the `CMakeParseArguments` module, which is where it used to live before becoming a built in command.

```cmake
function(COMPLEX)
    cmake_parse_arguments(
        COMPLEX_PREFIX
        "SINGLE;ANOTHER"
        "ONE_VALUE;ALSO_ONE_VALUE"
        "MULTI_VALUES"
        ${ARGN}
    )
endfunction()
```

- Inside the function after this call, you'll find:

```bash
complex(SINGLE ONE_VALUE value MULTI_VALUES some other values)
COMPLEX_PREFIX_SINGLE = TRUE
COMPLEX_PREFIX_ANOTHER = FALSE
COMPLEX_PREFIX_ONE_VALUE = "value"
COMPLEX_PREFIX_ALSO_ONE_VALUE = <UNDEFINED>
COMPLEX_PREFIX_MULTI_VALUES = "some;other;values"
```

- If you look at the official page, you'll see a slightly different method using set to avoid explicitly writing the semicolons in the list; feel free to use the structure you like best.
- You can mix it with the positional arguments listed above; any remaining arguments (therefore optional positional arguments) are in `COMPLEX_PREFIX_UNPARSED_ARGUMENTS`.


## Configure File

CMake allows you to access CMake variables from your code using `configure_file`.

- This command copies a file (traditionally ending in `.in`) from one place to another, **substituting all CMake variables it finds**.
- If you want to avoid replacing existing `${`} syntax in your input file, use the `@ONLY` keyword.
- There's also a `COPY_ONLY` keyword if you are just using this as a replacement for `file(COPY`.

This functionality is used quite frequently; for example, on Version.h.in:

```cpp
//Version.h.in
#pragma once

#define MY_VERSION_MAJOR @PROJECT_VERSION_MAJOR@
#define MY_VERSION_MINOR @PROJECT_VERSION_MINOR@
#define MY_VERSION_PATCH @PROJECT_VERSION_PATCH@
#define MY_VERSION_TWEAK @PROJECT_VERSION_TWEAK@
#define MY_VERSION "@PROJECT_VERSION@"
```

CMake lines:

```cmake
configure_file (
    "${PROJECT_SOURCE_DIR}/include/My/Version.h.in"
    "${PROJECT_BINARY_DIR}/include/My/Version.h"
)
```

You should include the binary include directory as well when building your project. If you want to put any true/false variables in a header, CMake has C specific `#cmakedefine` and `#cmakedefine01` replacements to make appropriate define lines.

You can also (and often do) use this to produce `.cmake` files, such as the configure files (see installing).

## Reading files

The other direction can be done too; you can read in something (like a version) from your source files.

- If you have a header only library that you'd like to make available with or without CMake, for example, then this would be the best way to handle a version. 

```cmake
# Assuming the canonical version is listed in a single line
# This would be in several parts if picking up from MAJOR, MINOR, etc.
set(VERSION_REGEX "#define MY_VERSION[ \t]+\"(.+)\"")

# Read in the line containing the version
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/include/My/Version.hpp"
    VERSION_STRING REGEX ${VERSION_REGEX})

# Pick out just the version
string(REGEX REPLACE ${VERSION_REGEX} "\\1" VERSION_STRING "${VERSION_STRING}")

# Automatically getting PROJECT_VERSION_MAJOR, My_VERSION_MAJOR, etc.
project(My LANGUAGES CXX VERSION ${VERSION_STRING})
```

- Above, `file(STRINGS file_name variable_name REGEX regex) `picks lines that match a regex; and the same regex is used to then pick out the parentheses capture group with the version part. Replace is used with back substitution to output only that one group.