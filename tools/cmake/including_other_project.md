# Including other projects

## Git Submodule Method

- If you want to add a Git repository on the same service (GitHub, GitLab, BitBucket, etc), the following is the correct Git command to set that up as a submodule in the extern directory:

```bash
gitbook $ git submodule add ../../owner/repo.git extern/repo
```

The relative path to the repo is important;

- it allows you to keep the same access method (ssh or https) as the parent repository. This works very well in most ways.
- When you are inside the submodule, you can treat it just like a normal repo, and
- when you are in the parent repository, you can "add" to change the current commit pointer.

But the traditional downside is that you either have to have your users know git submodule commands, so they can init and update the repo, or they have to add `--recursive` when they initially clone your repo. CMake can offer a solution:

```cmake
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/repo/CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()
```

- The first line checks for Git using CMake's built in `FindGit.cmake`.
- Then, if you are in a git checkout of your source, add an option (defaulting to `ON`) that allows developers to turn off the feature if they need t
- We then run the command to get all repositories, and fail if that command fails, with a nice error message
- Finally, we verify that the repositories exist before continuing, regardless of the method used to obtain them. You can use OR to list several.

- Now, your users can be completely oblivious to the existence of the submodules, and you can still keep up good development practices! The only thing to watch out for is for developers; you will reset the submodule when you rerun CMake if you are developing inside the submodule. Just add new commits to the parent staging area, and you'll be fine.

You can then include projects that provide good CMake support: `add_subdirectory(extern/repo)`
- Or, you can **build an interface library target yourself if it is a header only project**.
- Or, you can use `find_package` if that is supported, probably preparing the initial search directory to be the one you've added (check the docs or the file for the `Find*.cmake` file you are using).
- You can also include a CMake helper file directory if you append to your `CMAKE_MODULE_PATH`, for example to add `pybind11`'s improved `FindPython*.cmake` files.

## Git version number

Move this to Git section:

```cmake
execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                OUTPUT_VARIABLE PACKAGE_GIT_VERSION
                ERROR_QUIET
                OUTPUT_STRIP_TRAILING_WHITESPACE)

```

## Downloading Projects

**Downloading Method: build time**

- Until CMake 3.11, the primary download method for packages was done at build time. This causes several issues; most important of which is that `add_subdirectory` doesn't work on a file that doesn't exist yet! The built-in tool for this, `ExternalProject`, has to work around this by doing the build itself. (It can, however, build non-CMake packages as well). Note that `ExternalData` is the tool for non-package data.

**Downloading Method: configure time**

- If you prefer configure time, see the [Crascit/DownloadProject repository](https://github.com/Crascit/DownloadProject) for a drop-in solution.
- Submodules work so well, though, that I've discontinued most of the downloads for things like GoogleTest and moved them to submodules.
- Auto downloads are harder to mimic if you don't have internet access, and they are often implemented in the build directory, wasting time and space if you have multiple build directories.


## FetchContent (CMake 3.11+)

- Often, you would like to do your download of data or packages as part of the configure instead of the build. This was invented several times in third party modules, but was finally added to CMake itself as part of CMake 3.11 as the FetchContent module.

The `FetchContent` module has excellent documentation that I won't try to repeat. The key ideas are:

- Use `FetchContent_Declare(MyName)` to **get data or a package**. You can set URLs, Git repositories, and more.
- Use `FetchContent_GetProperties(MyName)` on the name you picked in the first step to get `MyName_*` variables.
- Check `MyName_POPULATED`, and if not populated, use `FetchContent_Populate(MyName)` (and if a package, `add_subdirectory("${MyName_SOURCE_DIR}" "${MyName_BINARY_DIR}")`)

For example, to download Catch2:

```cmake
FetchContent_Declare(
  catch
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v2.13.6
)

# CMake 3.14+
FetchContent_MakeAvailable(catch)
```

If you can't use CMake 3.14+, the classic way to prepare code was:

```cmake
# CMake 3.11+
FetchContent_GetProperties(catch)
if(NOT catch_POPULATED)
  FetchContent_Populate(catch)
  add_subdirectory(${catch_SOURCE_DIR} ${catch_BINARY_DIR})
endif()
```

Of course, you could bundled this up into a macro:

```cmake
if(${CMAKE_VERSION} VERSION_LESS 3.14)
    macro(FetchContent_MakeAvailable NAME)
        FetchContent_GetProperties(${NAME})
        if(NOT ${NAME}_POPULATED)
            FetchContent_Populate(${NAME})
            add_subdirectory(${${NAME}_SOURCE_DIR} ${${NAME}_BINARY_DIR})
        endif()
    endmacro()
endif()
```

Now you have the CMake 3.14+ syntax in CMake 3.11+. See the [example here](https://gitlab.com/CLIUtils/modern-cmake/-/tree/master/examples/fetch).