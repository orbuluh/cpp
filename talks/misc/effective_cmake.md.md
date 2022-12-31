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

## :bulb` Create macros to wrap commands that have output parameter, otherwise, create a function.

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
- 
