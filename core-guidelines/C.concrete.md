
# C.concrete: Concrete types

## C.10: Prefer concrete types over class hierarchies
- If a class is part of a hierarchy, we must manipulate its objects through pointers or references.
  - That implies more memory overhead, more allocations and deallocations, and more run-time overhead to perform the resulting indirections.
- Concrete types can be stack-allocated and be members of other classes.
- The use of indirection is fundamental for run-time polymorphic interfaces. The allocation/deallocation overhead is not (that's just the most common case).
  - We can use a base class as the interface of a scoped object of a derived class. This is done where dynamic allocation is prohibited (e.g. hard-real-time) and to provide a stable interface to some kinds of plug-ins

