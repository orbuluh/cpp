# Effective CMake - Daniel Pfeifer


## Why?

The way you use CMake affects your users!

## CMake's similarities with C++

- :whale: Big userbase, industry dominance
- :whale: Focus on backwards compatibility
- :whale: Complex, feature rich, "multi paradigm" for different use cases
- :weary: Bad reputation, "bloated", "horrible syntax"
- :weary: Some not very well known features

![](../pics/effective_cmake_fun_comic.JPG)


## :bulb: CMake is code.

- Use the same principles for `CmakeLists.txt` and modules as for the rest of your codebase.
- Don't repeat your self!

## Organization

- **Directories** that contain a `CmakeLists.txt` are the entry point for the build system generator.
- **Subdirectories** may be added with `add_subdirectory()` and must contain a `CmakeLists.txt` too.
- **Scripts** are `<script>.cmake` files that can be executed with `cmake -P <script>.cmake`. Not all commands are supported through script. (For example, `add_executable` is only allowed in project)
- **Modules** are `<script>.cmake` files located in the `CMAKE_MODULE_PATH`. Modules can be loaded with the `include()` command.

## Syntax

```txt
command_name(space separated list of string)
```

- Each identifier is a string in cmake

Types of commands:

- Scripting commands change state of command processor
  - Set variables
  - Change behavior of other commands
- Project commands
  - Create build targets
  - Modify build targets

**Note: Command invocations are not expressions.**

- You cannot  put a command invocation directly as argument of another command, or inside an if condition.


## Variables

```txt
set(hello world)
message(STATUS "hello, ${hello}")
```

- Set with the `set()` command
- Expand with `${}`
- Variables and values are strings - even lists
- Lists are `;`-separated strings.
- :rotating_light: **CMake variables are not (and separated to) environment variables** (unlike `Makefile`)
- Unset variable expands to empty string --> Source of most problems and it's often advocated to avoid variables

## Comments

```cmake
# a single line comment

#[==[
    multi line comments. The equal sign can be any number, just need to match
    #[=[
        nested comment
    #]=]
#]==]
```

## Generator expressions

```cmake
target_compile_definitions(foo PRIVATE "VERBOSITY=$<IF:$<CONFIG:Debug>,30,10>")
```

- Generator expressions use the `$<>` syntax
- Not evaluated by command interpreter. It is just a string with `$<>`
- Evaluated/expanded during build system generation, or say project generation step.
- Not supported in all commands - as it only got expanded during build system generation. Before then, it's just a string. So it won't be inside a if. As if has to evaluated during processing.
- Normally it should be used in place that you are modifying the build target.

## Two types of commands

- Commands can be added with `function()` or `macro()`: Difference is like in C++
- When a new command replaces an existing command (because names are the same), the old one can still be accessed with a `_` prefix

## Custom command/function

```cmake
function(my_command input output)
    # variables set inside the function remains in the function scope
    # unless you declare PARENT_SCOPE
    set(${output} ... PARENT_SCOPE)
endfunction()

my_command(foo bar)
```

- Variables are scoped to the function, unless set with `PARENT_SCOPE`
- Available variables:
  - `input`, `output`, `ARGC` (total number of arguments), `ARGV` (actual list of the arguments),
  - `ARGN` (the list of arguments we haven't assigned a name to), `ARG0`, `ARG1`, `ARG2`, ..., `ARG9` --> this helps us to define optional arguments
- Example: `${output}` **expands** to `bar`

Compared to you define it with `macro`:

```cmake
macro(my_command input output)
    #...
endmacro()
my_command(foo bar)
```

- No extra scope with macro
- Text replacements: ${input}, ${output}, ${ARGC}, ${ARGV}, ${ARGN}, ${ARG0}, ...${ARG9} will not be a text replacement, meaning if you check whether variable input exists, it will be false.
- Example: ${output} is **replaced** by bar

## :bulb: Create macros to wrap commands that have output parameter, otherwise, create a function.

- As macro won't introduce new scope.
- When you defined something inside macro, it will be visible outside of the macro.
- (Not totally understand yet) - "Because you don't know what the output will be set int the parent scope, you don't know what's the default, so you can wrap it in a macro. Then it will have the same side effect as the ref command"

## :bulb: Modern CMake is about Targets and Properties

- Variables and custom commands are so CMake 2.8.12
- We would like to deprecate them and evolve our CMake, how?


## deprecate custom command

```cmake
macro(my_command)
    message(DEPRECATION "The my_command command is deprecated!")
    _my_command(${ARGV}) # add prefix _ to use the original command
                         # forward the ${ARGV} accordingly
endmacro()
```

## deprecate variables

```cmake
set(hello "hello world!")

function(__deprecated_var var access)
    if (access STREQUAL "READ_ACCESS")
        message(DEPRECATION
            "The variable '${var}' is deprecated!")
    endif()
endfunction();

# build-in command, whenever hello variable is used, it will call the __deprecated_var function accordingly
variable_watch(hello __deprecated_var)
```

## Modern CMake: no variables!

```cmake
add_library(Foo foo.cpp)
target_link_libraries(Foo PRIVATE Bar::Bar)

if (WIN32)
    # add additional sources and dependent libs if platform required
    # (compared to the old way where you define a list for one platform then
    # another list for the other variable, if you happen to have typo, you build
    # nothing etc!)
    target_sources(Foo PRIVATE foo_win32.cpp)
    target_link_libraries(Foo PRIVATe Bar::Win32Support)
endif()
```

## :bulb: Avoid custom variables in the arguments of project commands!

## :bulb: Don't use `file(GLOB)` in projects

- The fundamental problem is **CMake is not a build system, it's a build system generator**
- File glob'ing in a build system is nice, because when you trigger a build system, it will evaluate the glob expression, and it will get the list of files.
- But CMake is different, CMake generate the build system, it evaluates the glob expression and gives you a list of files. But then in the end, for the actual build system, it would only get the files CMake provide. So when you actually run the build system, it will have no idea if something has changed.
- Can CMake not evaluate the glob and simply forward it to the build system? It can't because not all the build system supports glob. And CMake is trying to be the common denominator for all kinds of build system, hence it doesn't support to forward the glob.

## Think CMake as an object oriented programming language

- Imagine Targets as Objects
- Constructors: `add_executable()`, `add_library()`
- Member variables: All kinds of target properties
- Member functions: (calling these functions will modify the member variables, e.g. properties of the target)
  - `get_target_property()`
  - `set_target_property()`
  - `get_property(TARGET)`
  - `set_property(TARGET)`
  - `target_compile_definitions()`
  - `target_compile_features()`
  - `target_compile_options()`
  - `target_include_directories()`
  - `target_link_libraries()`
  - `target_sources()`

## Avoid these commands: `add_compile_options()`, `include_directories()`, `link_directories()`, `link_libraries()`

- These commands are used in directory level. All the target created in the directory will inherit those properties. This will just make the build complicated to understand.
- Always better to work on something that is on target level instead of directory level.

```cmake
target_compile_features(Foo
    PUBLIC
        cxx_strong_enums
    PRIVATE
        cxx_lambdas
        cxx_range_for
)
```

- Adds `cxx_strong_enums` to the target properties `COMPILE_FEATURES` and `INTERFACE_COMPILE_FEATURES`
- Adds `cxx_lambdas;cxx_range_for` to the target property `COMPILE_FEATURES`
- This tells CMake about the language features that you need inside the library

## :bulb: Get your hands off CMAKE_CXX_FLAGS

- These flags often broke in the future.
- Only tell compiler what feature you need (like above example with cxx_range_for, ...etc), then let CMake figure out what compiler flag it needs.

## Build specification and usage requirements

- Non-`INTERFACE_` properties define the build specification of a target
- `INTERFACE_` properties define the usage requirements of a target
- `PRIVATE` populates the Non-`INTERFACE_` properties
- `INTERFACE` populates the `INTERFACE_` properties
- `PUBLIC` populates **both**

## :bulb: Use `target_link_libraries()` to express direct dependencies

```cmake
target_link_libraries(Foo
PUBLIC Bar::Bar
PRIVATE Cow::Cow
)
```

- Adds `Bar::Bar` to the target properties `LINK_LIBRARIES` and `INTERFACE_LINK_LIBRARIES`
- Adds `Cow::Cow` to the target property `LINK_LIBRARIES`
- **Effectively** adds all `INTERFACE_<property>` of `Bar::Bar` to `<property>` and `INTERFACE_<property>`
- **Effectively** adds all `INTERFACE_<property>` of `Cow::Cow` to `<property>`
  - Saying "Effectively" because it's not what the `target_link_libraries` do, but what is done later when the dependencies are resolved.
- Also adds the generator `$<LINK_ONLY:Cow:Cow>` to `INTERFACE_LINK_LIBRARIES`
  - Because imagine you have a static library, which depends on another library. If you want to link to the static library, on the command line, you will see both the libraries that your target depends on plus the dependencies of the libraries are in the command.
  - On the contrary, in CMake, you just express this as the abstract interfaces. And therefore, CMake needs to know whether a target is LINK_ONLY

## Library that are purely for usage requirements/build specifications

```cmake
add_library(Bar INTERFACE)
target_compile_definitions(Bar INTERFACE BAR=1)
```

- `Bar` is actually not a library
- `INTERFACE` libraries have no build specification
- They only have usage requirements.
- Here, every executable or library that links to `Bar` would have the `BAR` variable defined as `1`
- This is very useful for header-only library. You create header-only library as a pure INTERFACE, you add `target_include_directories` as the property of the INTERFACE, then everyone who links (though not actually links, more like declares a dependency) to the pure interface "library" will therefore have the `target_include_directories` that contains those header.

## :bulb: Don't abuse requirements!

- Eg. `-Wall` is not a requirement to build a project

## Project boundaries

## How to use external libraries?

**Always** like this:

```cmake
find_package(Foo 2.0 REQUIRED)
#...
target_link_libraries(... Foo::Foo ...)
```

Question: If `Foo` is a static library that depends on other libraries, how should this looks like? It should looks exactly the same.

Question: If `Foo` is header-only library, how should it look like? Still the same.

Hence the "Always"

But then where is this `Foo` comes from? There should be a `FindFoo.cmake` somewhere...

## `FindFoo.cmake`

```cmake
find_path(Foo_INCLUDE_DIR foo.h)
find_library(Foo_LIBRARY foo)
mark_as_advanced(Foo_INCLUDE_DIR Foo_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Foo
    REQUIRED_VARS Foo_LIBRARY Foo_INCLUDE_DIR
    )

# Foo_FOUND set by find_package_handle_standard_args above

if(Foo_FOUND AND NOT TARGET Foo::Foo)
    # UNKNOWN means you don't know it's static or share lib
    add_library(Foo::Foo UNKNOWN IMPORTED)
    set_target_properties(Foo::Foo PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
        IMPORTED_LOCATION "${Foo_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${Foo_INCLUDE_DIR}"
        )
endif()
```

- This is a simple case, which doesn't handle version number, configurations (like debug build link to debug libs etc), ...etc.
- This is just a basic example of how `find_package` would do. Library user should not provide this. In reality, a `FindFoo.cmake` would look much longer and deals with all the thing.

## :bulb: Use a Find* module for **third party** libraries ~~that are not built with CMake~~ that do not support clients to use CMake. Also report this as a bug to the authors

## How to export your library interface with CMake?

- Below 3 snippets (though not as simple as author wants) are what you need for a library author

```cmake
find_package(Bar 2.0 REQUIRED)
add_library(Foo ...)
target_link_libraries(Foo PRIVATE Bar::Bar)

install(TARGETS Foo EXPORT FooTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
    )

install(EXPORT FooTargets
    FILE FooTargets.cmake
    NAMESPACE Foo::
    DESTINATION lib/cmake/Foo
    )
```

**Handle version:**

```cmake
include(CMakePackageConfigHelpers)
    write_basic_package_version_file("FooConfigVersion.cmake"
    VERSION ${Foo_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES "FooConfig.cmake" "FooConfigVersion.cmake"
    DESTINATION lib/cmake/Foo
    )
```

**Handle sub-dependencies of Foo**

```cmake
include(CMakeFindDependencyMacro)
find_dependency(Bar 2.0)
include("${CMAKE_CURRENT_LIST_DIR}/FooTargets.cmake")
```

:rotating_light: The library interface may change during installation. (E.g. when you install and when you build, they may be different) Use `BUILD_INTERFACE` and `INSTALL_INTERFACE` generator expressions as filters.

```cmake
target_include_directories(Foo PUBLIC
    $<BUILD_INTERFACE:${Foo_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${Foo_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )
```

## Creating packages

## CPack

- CPack is a packaging tool distributed with CMake
- `set()` variables in `CPackConfig.cmake` or
- `set()` variables in `CMakeLists.txt` and `include(CPack)`

## :bulb: Write your own `CPackConfig.cmake` and `include()` the one that is generated by CMake

## CPack secret

- The variable CPACK_INSTALL_CMAKE_PROJECTS is a list of quadruples:
  - Build directory
  - Project Name
  - Project Component
  - Directory (The location where it should be in the package)

## Packaging multiple configurations

1. Make sure different configurations don't collide

```cmake
# for example, for debug build:
set(CMAKE_DEBUG_POSTFIX "-d")
```

2. Create separate build directories for `debug`, `release`.
3. Use this CPackConfig.cmake (Whenever I want to make a package, I use a script to generate the cmake file like below)

```cmake
include("release/CPackConfig.cmake")
set(CPACK_INSTALL_CMAKE_PROJECTS
    "debug;Foo;ALL;/"   #install Foo project from debug directory
                        #take ALL the component, and put to the root (/) of directory
    "release;Foo;ALL;/" #install Foo project from release directory
                        #take ALL the component, and put to the root (/) of directory
    )
```

So the script needs to know where is the source directory, then the script create 2 build directories, one for `debug`, the other for `release`, then config and compile both, then at one level above, create the `CPackConfig.cmake`, then run `CPack` on it. Finally, take the result in build/release directory and put into the same package

## (Author's) requirements for a package manager:

- Support system packages (for example, if libc is in the system, package manager should not download again. It should work out of the box.)
- Support rebuilt libraries
- Support building dependencies as subprojects
- Do not require any changes to my projects.

It's possible that external library is ALWAYS like this:

```cmake
find_package(Foo 2.0 REQUIRED)
#...
target_link_libraries(... Foo::Foo ...)
```
## Do not require any changes to my projects!

- System packages ... it should work out of the box
- Prebuilt libraries ... need to be put into `CMAKE_PREFIX_PATH`
- Subprojects ...
  - We need to turn `find_package(Foo)` into a no-op
  - What about the imported target `Foo::Foo`?

## :bulb: When you export `Foo` in namespace `Foo::`, also create an alias `Foo::Foo`

```cmake
add_library(Foo::Foo ALIAS Foo)
```
- this means, using `Foo` inside the same build directory will look the same as if it's used as an external library.

## The toplevel super-project

```cmake
set(CMAKE_PREFIX_PATH "/prefix")
set(as_subproject Foo)

macro(find_package)
    if(NOT "${ARG0}" IN_LIST as_subproject)
        _find_package(${ARGV})
    endif()
endmacro()

add_subdirectory(Foo)
add_subdirectory(App)
```

With defining like this ... If `Foo` is a ...

- system package:
  - `find_package(Foo)` either finds FooConfig.cmake in the system or uses `FindFoo.cmake` to find the library in the system. In either case, the target `Foo::Foo` is imported.
- prebuilt library:
  - `find_package(Foo)` either finds `FooConfig.cmake` in the `CMAKE_PREFIX_PATH` or uses `FindFoo.cmake` to find the library in the `CMAKE_PREFIX_PATH`. In either case, the target `Foo::Foo` is imported.
- subproject:
  - `find_package(Foo)` does nothing. The target `Foo::Foo` is part of the project.

## CTest

## Run with `ctest -S build.cmake`

- CTest knows how to run coverage, how to run memcheck, even how to parse the output of those tools
- All the special flags for test should be outside of your project. Just isolate them in a .cmake file contains below.

```cmake
set(CTEST_SOURCE_DIRECTORY "/source")
set(CTEST_BINARY_DIRECTORY "/binary")

set(ENV{CXXFLAGS} "--coverage")
set(CTEST_CMAKE_GENERATOR "Ninja")
set(CTEST_USE_LAUNCHERS 1)

set(CTEST_COVERAGE_COMMAND "gcov")
set(CTEST_MEMORYCHECK_COMMAND "valgrind")
#set(CTEST_MEMORYCHECK_TYPE "ThreadSanitizer")

ctest_start("Continuous")
ctest_configure()
ctest_build()
ctest_test()
ctest_coverage()
ctest_memcheck()
ctest_submit()
```

## :bulb: CTest scripts are the right place for CI specific settings.

- Keep that information out of the project


## Filtering tests by name

Define like this:

```cmake
add_test(NAME Foo.Test
    COMMAND foo_test --number 0
    )
```

Run like this:

```bash
ctest -R 'Foo.' -j4 --output-on-failure
```

## :bulb: Follow a naming convention for test names. This simplifies filtering by regex.

## Test on "fail to compile"

```cmake
add_library(foo_fail STATIC EXCLUDE_FROM_ALL
    foo_fail.cpp
    )
# try to build the project and only when you run ctest
# then it trigger the build command accordingly to test the
# supposed-to-fail-to-build target
add_test(NAME Foo.Fail
    COMMAND ${CMAKE_COMMAND}
    --build ${CMAKE_BINARY_DIR}
    --target foo_fail
    )
# set the test property that the above command should fail and it should fail
# only if it generates certain static assert message
set_property(TEST Foo.Fail PROPERTY
    PASS_REGULAR_EXPRESSION "static assert message"
    )
```

## Running cross-compiled tests

- When the testing command is a build target, the command line is prefixed with `${CMAKE_CROSSCOMPILING_EMULATOR}`.
- When crosscompiling from Linux to Windows,
set `CMAKE_CROSSCOMPILING_EMULATOR` to `wine`.
- When crosscompiling to ARM, set `CMAKE_CROSSCOMPILING_EMULATOR` to `qemu-arm`.
- To run tests on another machine, set `CMAKE_CROSSCOMPILING_EMULATOR` to a script that copies it over and executes it there


Run tests on real hardware like this:

```bash
#!/bin/bash
tester=$1
shift
# create temporary file
filename=$(ssh root@172.22.22.22 mktemp)
# copy the tester to temporary file
scp $tester root@172.22.22.22:$filename
# make test executable
ssh root@172.22.22.22 chmod +x $filename
# execute test
ssh root@172.22.22.22 $filename "$@"
# store success
success=$?
# cleanup
ssh root@172.22.22.22 rm $filename
exit $success
```

## More on cross-compiling

- Cross-compiling is done through toolchain in cmake
- Example of things that should be in a `Toolchain.cmake` file:

```cmake
set(CMAKE_SYSTEM_NAME Windows)

set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_CROSSCOMPILING_EMULATOR wine64)
```

## :bulb: Don't put logic in toolchain file

- Should be as simple as above example
- Single toolchain file per target platform you want to build


## Static Analysis

## :bulb: Treat warnings as errors?

- How do you treat build errors? You fix them, you reject pull requests, you hold off releases.
- To treat warnings as errors, never  pass `-Werror` to the compiler
- If you do, your compiler treats warnings as errors.
- You can no longer treat warnings as errors, because you will no longer get any warnings.  All you get is errors!

## `-Werror causes pain`

- You cannot enable `-Werror` unless you already reached zero warnings
- You cannot increase the warning level unless you already fixed all warnings introduced yb  that level.
- You cannot upgrade your compiler unless you already fixed all new warnings that the compiler reports at your warning level.
- You cannot update your dependencies unless you already ported your code away from any symbols that are now `[[deprecated]]`
- You cannot `[[deprecated]]` your internal code as long as it is still used. But once it is no longer used, you can as well just remove it...

## Better: Treat **new** warnings as errors!

1. At hte beginning of a development cycle (e.g. sprint), allow new warnings to be introduced.

- increase warning level, enable new warnings explicitly
- update the compiler
- update dependencies
- Mark symbols as `[[deprecated]]`

2. Then, burn down the number of warnings
3. Repeat

## Tools

- **clang-tidy** is a clang-based C++ “linter” tool. Its purpose is to provide an extensible framework for diagnosing and fixing typical programming errors, like style violations, interface misuse, or bugs that can be deduced via static analysis.
- **cpplint** is automated checker to make sure a C++ file follows Google’s C++ style guide.
- **include-what-you-use** analyzes #includes in C and C++ source
files.
- **clazy** is a clang wrapper that finds common C++/Qt antipatterns that decrease performance

## Using tools with CMake


- `<lang>_CLANG_TIDY`
- `<lang>_CPPLINT`
- `<lang>_INCLUDE_WHAT_YOU_USE`
  -  Runs the respective tool along the with compiler.
  - Diagnostics are visible in your IDE.
  - Diagnostics are visible on CDash.
- `LINK_WHAT_YOU_USE`:
  - links with `-Wl`,`--no-as-needed`, then runs `ldd -r -u`.

- `<lang>` is either `C` or `CXX`.
- Each of those properties is initialized with `CMAKE_<property>`.

## Caveat of scanning header files

- Most of those tools report diagnostics for the current source file plus the associated header.
- Header files with no associated source file will not be analyzed.
- You may be able to set a **custom header filter**, but then the headers may be analyzed multiple times.

## :bulb: For each header file, there is an associated source file that #includes this header file at the top. EVEN if that source file would otherwise be empty.

- Create associated sources file
```bash
#!/usr/bin/env bash
for fn in `comm -23 \
    <(ls *.h|cut -d '.' -f 1|sort) \
    <(ls *.c *.cpp|cut -d '.' -f 1|sort)`
do
    echo "#include \"$fn.h\"" >> $fn.cpp
done
```

## How to enable warnings from from outside the CMake project

```bash
env CC=clang CXX=clazy cmake \
    -DCMAKE_CXX_CLANG_TIDY:STRING=\
    'clang-tidy;-checks=-*,readability-*' \
    -DCMAKE_CXX_INCLUDE_WHAT_YOU_USE:STRING=\
    'include-what-you-use;-Xiwyu;--mapping_file=/iwyu.imp' \
    ..
```

## Supported by all IDEs

- Just setting `CMAKE_CXX_CLANG_TIDY` will make all `clang-tidy` diagnostics appear in your normal build output.
- No special IDE support needed.
- If IDE understands fix-it hints from `clang` it will also understand the ones from `clang-tidy`