# C.ctor: Constructors, assignments, and destructors
These are default operations:

- a default constructor: `X()`
- a destructor: `~X()`
- a copy constructor: `X(const X&)`
- a move constructor: `X(X&&)`
- a copy assignment: `X& operator=(const X&)`
- a move assignment: `X& operator=(X&&)`

- By default, the compiler defines each of these operations if it is used, but the default can be suppressed.
- The default operations are a set of related operations that together implement the lifecycle semantics of an object.
- By default, C++ treats classes as value-like types, but not all types are value-like.

- [C.defop: Default Operations](C.defop.md)
- [C.dtor: Destructors](C.dtor.md)
- ...