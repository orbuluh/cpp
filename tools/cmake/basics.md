# CMake basics

## Minimum Version

The required first line of every `CMakeLists.txt`: `cmake_minimum_required(VERSION 3.1)`

- command name `cmake_minimum_required` is case insensitive, so the common practice is to use lower case.
- The `VERSION` is a special **keyword** for this function.
- And the **value** of the version follows the keyword.

The version of CMake will also dictate the **policies**, which define **behavior changes** -> check [official doc](https://cmake.org/cmake/help/latest/manual/cmake-policies.7.html)


- Starting in CMake 3.12, this supports a range, such as `VERSION 3.1...3.15`
- New versions of policies tend to be most important for macOS and Windows users, who also usually have a very recent version of CMake.

- This is what new projects should do

```cmake
cmake_minimum_required(VERSION 3.7...3.25)`

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()
```

## Setting a project

Now, every top-level CMake file will have the next line:

```cmake
project(MyProject VERSION 1.0
                  DESCRIPTION "Very nice project"
                  LANGUAGES CXX)
```

Now we see even more syntax.

- Strings are quoted
- whitespace doesn't matter, and
- the name of the project is the first argument (positional).
- All the keyword arguments here are optional.
- The version sets a bunch of variables, like `MyProject_VERSION` and `PROJECT_VERSION`.
- The languages are `C`, `CXX`, `Fortran`, `ASM`, `CUDA` (CMake 3.8+), `CSharp` (3.8+), and `SWIFT` (CMake 3.15+ experimental).`C` `CXX` is the default.
- In CMake 3.9, `DESCRIPTION` was added to set a project description, as well. The documentation for project may be helpful.
- You can add comments with the `#` character. CMake does have an inline syntax for comments too, but it's rarely used.
- There's really nothing special about the project name. No targets are added at this point.

## Making an executable

`add_executable(one two.cpp three.h)`

- There are several things to unpack here. `one` is both the name of the executable file generated, and the name of the **CMake target** created.
- The source file list comes next, and you can list as many as you'd like.
- CMake is smart, and will **only compile source file extensions**.
- The **headers will be, for most intents and purposes, ignored**; the only reason to list them is to get them to show up in IDEs.
- **Targets show up as folders in many IDEs.**
- More about the general build system and targets is available at [buildsystem](https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html).


## Making a library

`add_library(one STATIC two.cpp three.h)`

- You get to pick a type of library, `STATIC`, `SHARED`, or `MODULE`.
- If you leave this choice off, the value of `BUILD_SHARED_LIBS` will be used to pick between STATIC and SHARED.

- Often you'll need to make a fictional target, that is, one where nothing needs to be compiled, for example, **for a header-only library**. That is called an `INTERFACE` library, and is another choice; the only difference is it cannot be followed by filenames.

- You can also make an **ALIAS** library with an existing library, which simply gives you a new name for a target.
- The one benefit to this is that you can make libraries with `::` in the name (which you'll see later)

## Targets are your friend

Now we've specified a target `one` through `add_library(one STATIC two.cpp three.h)`, how do we add information about it?

For example, maybe it needs an `include` directory: `target_include_directories(one PUBLIC include)`

- `target_include_directories` adds an `include` directory to a target.
- `PUBLIC` doesn't mean much for an executable;
- for a library it lets CMake know that **any targets that link to this target must also need that `include` directory**.
- Other options are `PRIVATE` (only affect the current target, not dependencies), and `INTERFACE` (only needed for dependencies).

We can then chain targets:

`add_library(another STATIC another.cpp another.h)`
`target_link_libraries(another PUBLIC one)`

`target_link_libraries` is probably the most useful and confusing command in CMake.

- It takes a target (`another`) and adds a dependency if a target is given.
- If no target of that name (`one`) exists, then it adds a link to a library called `one` on your path (hence the name of the command).
- Or you can give it a full path to a library.
- Or a linker flag.
- Just to add a final bit of confusion, classic CMake allowed you to skip the keyword selection of `PUBLIC`, etc. If this was done on a target, you'll get an error if you try to mix styles further down the chain.
- **Focus on using targets everywhere, and keywords everywhere, and you'll be fine.**

Targets can have **include directories**, **linked libraries** (or **linked targets**), **compile options**, **compile definitions**, **compile features** (see the C++11 chapter), and more.

- As you'll see in the two including projects chapters, **you can often get targets (and always make targets) to represent all the libraries you use.**
- Even things that are not true libraries, like OpenMP, can be represented with targets.
- This is why Modern CMake is great!


## Variables and the Cache

:star2: **Local Variables**

A **local variable** is set like this: `set(MY_VARIABLE "value")`

- The names of **variables are usually all caps**, and the value follows.
- You access a variable by using `${}`, such as `${MY_VARIABLE}`.
- CMake has the concept of scope; **you can access the value of the variable after you set it as long as you are in the same scope**. If you leave a function or a file in a sub directory, the variable will no longer be defined.
- You can set a variable in the scope immediately above your current one with `PARENT_SCOPE` at the end.

**Lists** are simply a series of values when you set them: `set(MY_LIST "one" "two")`

- which internally become `;` separated values.
- So this is an identical statement: `set(MY_LIST "one;two")`

- The `list(` command has utilities for working with lists, and
- `separate_arguments` will turn a space separated string into a list (in place).
- Note that an **unquoted value in CMake is the same as a quoted one if there are no spaces in it;** this allows you to skip the quotes most of the time when working with value that you know could not contain spaces.

- When a variable is expanded using `${}` syntax, all the same rules about spaces apply.
- Be **especially careful with paths; paths may contain a space at any time and should always be quoted when they are a variable** (never write `${MY_PATH}`, always should be `"${MY_PATH}"`).

:star2: **Cache Variables**

If you want to set a variable from the command line, CMake offers a variable cache.

- Some variables are already here, like `CMAKE_BUILD_TYPE`.
- The syntax for declaring a variable and setting it if it is not already set is:

`set(MY_CACHE_VARIABLE "VALUE" CACHE STRING "Description")`

- This will not replace an existing value. This is so that you can set these on the command line and not have them overridden when the CMake file executes. If you want to use these variables as a make-shift global variable, then you can do:

```cmake
set(MY_CACHE_VARIABLE "VALUE" CACHE STRING "" FORCE)
mark_as_advanced(MY_CACHE_VARIABLE)
```

- The first line will cause the value to be set no matter what, and
- the second line will keep the variable from showing up in the list of variables if you run cmake -L .. or use a GUI.
- This is so common, you can also use the `INTERNAL` type to do the same thing (though technically it forces the STRING type, this won't affect any CMake code that depends on the variable):

```cmake
set(MY_CACHE_VARIABLE "VALUE" CACHE INTERNAL "")
```

Since BOOL is such a common variable type, you can set it more succinctly with the shortcut:

```cmake
option(MY_OPTION "This is settable from the command line" OFF)
```

- For the BOOL datatype, there are several different wordings for `ON` and `OFF`. See [cmake-variables](https://cmake.org/cmake/help/latest/manual/cmake-variables.7.html) for a listing of known variables in CMake.

:star2: **Environment variables**

- You can also `set(ENV{variable_name} value)` and` get $ENV{variable_name}` environment variables, though it is **generally a very good idea to avoid them.**

:star2: The Cache

- The cache is actually just a text file, `CMakeCache.txt`, that gets created in the build directory when you run CMake.
- This is how CMake remembers anything you set, so you don't have to re-list your options every time you rerun CMake.

## Properties

- The other way CMake stores information is in properties. This **is like a variable, but it is attached to some other item, like a directory or a target.**
- A global property can be a useful uncached global variable.
- Many target properties are initialized from a matching variable with `CMAKE_` at the front.
- So setting `CMAKE_CXX_STANDARD`, for example, will mean that all new targets created will have `CXX_STANDARD` set to that when they are created.
- There are two ways to set properties:

```cmake
set_property(TARGET TargetName
             PROPERTY CXX_STANDARD 11)

set_target_properties(TargetName PROPERTIES
                      CXX_STANDARD 11)
```

- The first form is more general, and can set multiple targets/files/tests at once, and has useful options.
- The second is a shortcut for setting **several properties on one target**. And you can get properties similarly:

```cmake
get_property(ResultVariable TARGET TargetName PROPERTY CXX_STANDARD)
```

- See [cmake-properties](https://cmake.org/cmake/help/latest/manual/cmake-properties.7.html) for a listing of all known properties. You can also make your own in some cases.
