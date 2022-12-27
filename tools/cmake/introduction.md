# CMake introduction

## CMake-like build system can solve:

- You want to avoid hard-coding paths
- You need to build a package on more than one computer
- You want to use CI (continuous integration)
- You need to support different OSs (maybe even just flavors of Unix)
- You want to support multiple compilers
- You want to use an IDE, but maybe not all of the time
- You want to describe how your program is structured logically, not flags and commands
- You want to use a library
- You want to use tools, like Clang-Tidy, to help you code
- You want to use a debugger

## Why must the answer be CMake?

- Every IDE supports CMake (or CMake supports that IDE). More packages use CMake than any other system. So, if you use a library that is designed to be included in your code, ... that will almost always include CMake. And that will quickly be the common denominator if you include multiple projects.
- And, if you need a library that's preinstalled, the chances of it having a find CMake script or config CMake script are excellent.

## Building a project

```bash
# Classic CMake Build Procedure (TM)
~/package $ cmake -S . -B build
~/package $ cmake --build build

# Any one of these commands will install:

## From the build directory (pick one)

~/package/build $ make install
~/package/build $ cmake --build . --target install
~/package/build $ cmake --install . # CMake 3.15+ only

## From the source directory (pick one)

~/package $ make -C build install
~/package $ cmake --build build --target install
~/package $ cmake --install build # CMake 3.15+ only
```

**Under source and do `--build` or under build directory?**

- You should try to get used to using `--build`, as that will free you from using only make to build.
- Note that working from the build directory is historically much more common, and **some tools and commands (including CTest <3.20) still require running from the build directory.**
- If you use `cmake --build` instead of directly calling the underlying build system, you can use `-v` for verbose builds (CMake 3.14+), `-j N` for parallel builds on N cores (CMake 3.12+), and `--target` (any version of CMake) or `-t` (CMake 3.15+) to pick a target.


## Options

> A Variable in CMake can be marked as "cached", which means it will be written to the cache (a file called `CMakeCache.txt` in the build directory) when it is encountered.

- You can preset (or change) the value of a cached option on the command line with `-D`.
- When CMake looks for a cached variable, it will use the existing value and will not overwrite it.

**Standard options**

These are common CMake options to most packages:

- `-DCMAKE_BUILD_TYPE=` Pick from `Release`, `RelWithDebInfo`, `Debug`, or sometimes more.
- `-DCMAKE_INSTALL_PREFIX=` The location to install to. System install on UNIX would often be /usr/local (the default), user directories are often ~/.local, or you can pick a folder.
- `-DBUILD_SHARED_LIBS=` You can set this ON or OFF to control the default for shared libraries (the author can pick one vs. the other explicitly instead of using the default, though)
- `-DBUILD_TESTING=` This is a common name for enabling tests, not all packages use it, though, sometimes with good reason.


## Debugging your CMake files

- `cmake --build build --verbose # CMake 3.14+ only` can make verbose output for the build. (Or setting env: `VERBOSE=1`)
- The `--trace` option will print every line of CMake that is run. Since this is very verbose, CMake 3.7 added `--trace-source="filename"`, which will print out every executed line of just the file you are interested in when it runs.
- If you select the name of the file you are interested in debugging (usually by selecting the parent directory when debugging a CMakeLists.txt, since all of those have the same name), you can just see the lines that run in that file.