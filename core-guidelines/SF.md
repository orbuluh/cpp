# SF: Source files
- Distinguish between declarations (used as interfaces) and definitions (used as implementations).
- Use header files to represent interfaces and to emphasize logical structure.

- [SF: Source files](#sf-source-files)
  - [SF.1: Use a .cpp suffix for code files and .h for interface files if your project doesn't already follow another convention](#sf1-use-a-cpp-suffix-for-code-files-and-h-for-interface-files-if-your-project-doesnt-already-follow-another-convention)
  - [SF.2: A .h file must not contain object definitions or non-inline function definitions](#sf2-a-h-file-must-not-contain-object-definitions-or-non-inline-function-definitions)
  - [SF.3: Use .h files for all declarations used in multiple source files](#sf3-use-h-files-for-all-declarations-used-in-multiple-source-files)
  - [SF.4: Include .h files before other declarations in a file](#sf4-include-h-files-before-other-declarations-in-a-file)
  - [SF.5: A .cpp file must include the .h file(s) that defines its interface](#sf5-a-cpp-file-must-include-the-h-files-that-defines-its-interface)
  - [SF.6: Use using namespace directives **for transition**, for **foundation libraries (such as `std`)**, or **within a local scope (only)**](#sf6-use-using-namespace-directives-for-transition-for-foundation-libraries-such-as-std-or-within-a-local-scope-only)
  - [SF.7: Don't write `using namespace` at global scope in a header file](#sf7-dont-write-using-namespace-at-global-scope-in-a-header-file)
  - [SF.8: Use `#include` guards for all `.h` files](#sf8-use-include-guards-for-all-h-files)
  - [SF.9: Avoid cyclic dependencies among source files](#sf9-avoid-cyclic-dependencies-among-source-files)
  - [SF.10: Avoid dependencies on implicitly #included names](#sf10-avoid-dependencies-on-implicitly-included-names)
  - [SF.11: Header files should be self-contained](#sf11-header-files-should-be-self-contained)
  - [SF.12: Prefer the quoted form of `#include` for files relative to the including file and the **angle bracket form everywhere else**](#sf12-prefer-the-quoted-form-of-include-for-files-relative-to-the-including-file-and-the-angle-bracket-form-everywhere-else)
  - [SF.20: Use namespaces to express logical structure](#sf20-use-namespaces-to-express-logical-structure)
  - [SF.21: Don't use an unnamed (anonymous) namespace in a header](#sf21-dont-use-an-unnamed-anonymous-namespace-in-a-header)
  - [SF.22: Use an unnamed (anonymous) namespace **for all internal/non-exported entities**](#sf22-use-an-unnamed-anonymous-namespace-for-all-internalnon-exported-entities)

## SF.1: Use a .cpp suffix for code files and .h for interface files if your project doesn't already follow another convention
- It's a longstanding convention. But consistency is more important, so if your project uses something else, follow that.
- Note: This convention reflects a common use pattern:
  - **Headers are more often shared with C** to compile as both C++ and C, which typically uses .h, and it's easier to name all headers .h instead of having different extensions for just those headers that are intended to be shared with C.
  - On the other hand, **implementation files are rarely shared with C** and so should typically be distinguished from .c files, so it's normally best to name all C++ implementation files something else (such as .cpp).
- The specific names .h and .cpp are not required (just recommended as a default) and other names are in widespread use. Examples are .hh, .C, and .cxx. Use such names equivalently.
- In this document, we refer to .h and .cpp as a shorthand for header and implementation files, even though the actual extension might be different.

## SF.2: A .h file must not contain object definitions or non-inline function definitions
- Including entities subject to the one-definition rule leads to linkage errors.

- Alternative formulation: A .h file must contain only:
  - #includes of other .h files (possibly with include guards)
  - templates
  - class definitions
  - function declarations
  - extern declarations
  - inline function definitions
  - constexpr definitions
  - const definitions
  - using alias definitions


## SF.3: Use .h files for all declarations used in multiple source files
- Maintainability. Readability.

```cpp
Example, bad
// bar.cpp:
void bar() { cout << "bar\n"; }

// foo.cpp:
extern void bar();
void foo() { bar(); }
```
- A maintainer of `bar` cannot find all declarations of bar if its type needs changing.
- The user of `bar` cannot know if the interface used is complete and correct.
- At best, error messages come (late) from the linker.

## SF.4: Include .h files before other declarations in a file
- Minimize context dependencies and increase readability.
- Note: This applies to both .h and .cpp files.
- Note: There is an argument for insulating code from declarations and macros in header files by #including headers after the code we want to protect (as in the example labeled "bad").
  - However that only works for one file (at one level): Use that technique in a header included with other headers and the vulnerability reappears.
  - a namespace (an "implementation namespace") can protect against many context dependencies.
  - full protection and flexibility require modules.


## SF.5: A .cpp file must include the .h file(s) that defines its interface
- This enables the compiler to do an early consistency check.

```cpp
// Example, bad
// foo.h:
void foo(int);
int bar(long);
int foobar(int);

// foo.cpp:
void foo(int) { /* ... */ }
int bar(double) { /* ... */ }
double foobar(int);
```
- The errors will not be caught until link time for a program calling `bar` or `foobar`.

```cpp
// Example
// foo.h:
void foo(int);
int bar(long);
int foobar(int);

// foo.cpp:
#include <foo.h>

void foo(int) { /* ... */ }
int bar(double) { /* ... */ }
double foobar(int);   // error: wrong return type
```

- The return-type error for `foobar` is now caught immediately when `foo.cpp` is compiled.
- The argument-type error for `bar` cannot be caught **until link time because of the possibility of overloading**, but **systematic use of .h files increases the likelihood that it is caught earlier by the programmer.**


## SF.6: Use using namespace directives **for transition**, for **foundation libraries (such as `std`)**, or **within a local scope (only)**
- `using namespace` can lead to name clashes, so it should be used sparingly.
  - Note: Don't write `using namespace` at global scope in a header file.
- Note: **A .cpp file is a form of local scope.**
  - There is little difference in the opportunities for name clashes in
    - an N-line .cpp containing a `using namespace X`,
    - an N-line function containing a `using namespace X`, and
    - M functions each containing a `using namespace X` with N lines of code in total.
- However, it is not always possible to qualify every name from a namespace in user code (e.g., during transition) and sometimes a namespace is so fundamental and prevalent in a code base, that consistent qualification would be verbose and distracting.

```cpp
#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <algorithm>

using namespace std;
```

- Here (obviously), the standard library is used pervasively and apparently no other library is used, so requiring `std::` everywhere could be distracting.
- Example: The use of `using namespace std;` leaves the programmer open to a name clash with a name from the standard library

```cpp
#include <cmath>
using namespace std;

int g(int x)
{
    int sqrt = 7;
    // ...
    return sqrt(x); // error
}
```
- However, this is not particularly likely to lead to a resolution that is not an error and people who use `using namespace std` are supposed to know about `std` and about this risk.


## SF.7: Don't write `using namespace` at global scope in a header file
- Doing so takes away an #includer's ability to effectively disambiguate and to use alternatives. It also makes #included headers order-dependent as they might have different meaning when included in different orders.
```cpp
// Example
// bad.h
#include <iostream>
using namespace std; // bad

// user.cpp
#include "bad.h"

bool copy(/*... some parameters ...*/);    // some function that happens to be named copy

int main()
{
    copy(/*...*/);    // now overloads local ::copy and std::copy, could be ambiguous
}
```
- Note: An exception is **using namespace std::literals**;. This is necessary to use string literals in header files and given the rules - users are required to name their own UDLs `operator""_x` - they will not collide with the standard library.


## SF.8: Use `#include` guards for all `.h` files
- To avoid files being #included several times.
- In order to avoid include guard collisions, do not just name the guard after the filename. Be sure to also include a key and good differentiator, such as the name of library or component the header file is part of.

```cpp
// file foobar.h:
#ifndef LIBRARY_FOOBAR_H
#define LIBRARY_FOOBAR_H
// ... declarations ...
#endif // LIBRARY_FOOBAR_H
```

- Note: Some implementations offer vendor extensions like `#pragma once` as alternative to include guards.
  - It is not standard and it is not portable.
  - It injects the hosting machine's filesystem semantics into your program, in addition to locking you down to a vendor.
  - Our recommendation is to write in ISO C++: See rule P.2.

## SF.9: Avoid cyclic dependencies among source files
- Cycles complicate comprehension and slow down compilation. They also complicate conversion to use language-supported modules (when they become available).

```cpp
Example, bad
// file1.h:
#include "file2.h"

// file2.h:
#include "file3.h"

// file3.h:
#include "file1.h"
```


## SF.10: Avoid dependencies on implicitly #included names
- Avoid surprises. Avoid having to change #includes if an #included header changes.
- Avoid accidentally becoming dependent on implementation details and logically separate entities included in a header.
```cpp
// Example, bad
#include <iostream>
using namespace std;

void use()
{
    string s;
    cin >> s;               // fine
    getline(cin, s);        // error: getline() not defined
    if (s == "surprise") {  // error == not defined
        // ...
    }
}
```
- `<iostream>` exposes the definition of `std::string` ("why?" makes for a fun trivia question), but it is not required to do so by transitively including the entire `<string>` header, resulting in the popular beginner question "why doesn't `getline(cin,s);` work?" or even an occasional "strings cannot be compared with `==`").

- The solution is to explicitly `#include <string>`:

```cpp
//Example, good
#include <iostream>
#include <string>
using namespace std;

void use()
{
    string s;
    cin >> s;               // fine
    getline(cin, s);        // fine
    if (s == "surprise") {  // fine
        // ...
    }
}
```
- Note: Some headers exist exactly to collect a set of consistent declarations from a variety of headers. For example:
  - a user can now get that set of declarations with a single `#include "basic_std_lib.h"`
  - This rule against implicit inclusion is not meant to prevent such deliberate aggregation.
```cpp
// basic_std_lib.h:
#include <string>
#include <map>
#include <iostream>
#include <random>
#include <vector>
```

## SF.11: Header files should be self-contained
- Usability, headers should be simple to use and work when included on their own.
- Headers should encapsulate the functionality they provide.
- Avoid clients of a header having to manage that header's dependencies.

```cpp
//Example
#include "helpers.h"
// helpers.h depends on std::string and includes <string>
```
- Note: Failing to follow this results in difficult to diagnose errors for clients of a header.
- Note: A header should include all its dependencies. Be careful about using relative paths because C++ implementations diverge on their meaning.


## SF.12: Prefer the quoted form of `#include` for files relative to the including file and the **angle bracket form everywhere else**
- The standard provides flexibility for compilers to implement the two forms of `#include` selected using the angle (`<>)` or quoted (`""`) syntax.
- Vendors take advantage of this and **use different search algorithms and methods for specifying the include path.**

- Nevertheless, the guidance is to use the quoted form for including files that exist at a relative path to the file containing the `#include` statement (from within the same component or project) and to use the angle bracket form everywhere else, where possible.
- This encourages being clear about **the locality of the file relative to files that include it**, or scenarios where the different search algorithm is required.
- It makes it easy to understand at a glance whether a header is being included from a local relative file versus a standard library header or a header from the alternate search path (e.g. a header from another library or a common set of includes).

```cpp
// foo.cpp:
#include "foo.h"                 // A file locally relative to foo.cpp in the same project, use the "" form
#include "foo_utils/utils.h"     // A file locally relative to foo.cpp in the same project, use the "" form

#include <component_b/bar.h>     // A file in the same project located via a search path, use the <> form
#include <some_library/common.h> // A file that is not locally relative, included from another library; use the <> form
#include <string>                // From the standard library, requires the <> form
```
- Note: Failing to follow this results in difficult to diagnose errors due to picking up the wrong file by incorrectly specifying the scope when it is included.
  - For example, in a typical case where the #include "" search algorithm might search for a file existing at a local relative path first, then using this form to refer to a file that is not locally relative could mean that if a file ever comes into existence at the local relative path (e.g. the including file is moved to a new location), it will now be found ahead of the previous include file and the set of includes will have been changed in an unexpected way.

- Library creators should **put their headers in a folder and have clients include those files using the relative path** `#include <some_library/common.h>`


## SF.20: Use namespaces to express logical structure
- empty

## SF.21: Don't use an unnamed (anonymous) namespace in a header
- It is almost always a bug to mention an unnamed namespace in a header file.

## SF.22: Use an unnamed (anonymous) namespace **for all internal/non-exported entities**
- Nothing external can depend on an entity in a nested unnamed namespace.
- Consider putting every definition in an implementation source file in an unnamed namespace unless that is defining an "external/exported" entity.

```cpp
// Example; bad
static int f();
int g();
static bool h();
int k();
// Example; good
namespace {
    int f();
    bool h();
}
int g();
int k();
```
- Example: An API class and its members can't live in an unnamed namespace; but **any "helper" class or function that is defined in an implementation source file should be at an unnamed namespace scope.**
