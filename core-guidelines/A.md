# A: Architectural ideas

## A.1: Separate stable code from less stable code
- Isolating less stable code facilitates its unit testing, interface improvement, refactoring, and eventual deprecation.

## A.2: Express potentially reusable parts as a library
- A library is a collection of declarations and definitions maintained, documented, and shipped together.
- A library could be a set of headers (a "header-only library") or a set of headers plus a set of object files.
- You can statically or dynamically link a library into a program, or you can `#include` a header-only library.

## A.4: There should be no cycles among libraries
- A cycle complicates the build process.
- Cycles are hard to understand and might introduce indeterminism (unspecified behavior).
- Note: A library can contain cyclic references in the definition of its components.
  - However, a library should not depend on another that depends on it.

