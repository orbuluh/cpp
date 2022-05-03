
# C.concrete: Concrete types

## C.10: Prefer concrete types over class hierarchies
- If a class is part of a hierarchy, we must manipulate its objects through pointers or references.
  - That implies more memory overhead, more allocations and deallocations, and more run-time overhead to perform the resulting indirections.
- Concrete types can be stack-allocated and be members of other classes.
- The use of indirection is fundamental for run-time polymorphic interfaces. The allocation/deallocation overhead is not (that's just the most common case).
  - We can use a base class as the interface of a scoped object of a derived class. This is done where dynamic allocation is prohibited (e.g. hard-real-time) and to provide a stable interface to some kinds of plug-ins

## C.11: Make concrete types regular
- The regular concept specifies that a type is regular, that is, it is copyable, default constructible, and equality comparable.
- Regular types are easier to understand and reason about than types that are not regular.
- The C++ built-in types are regular, and so are standard-library classes such as string, vector, and map. Concrete classes without assignment and equality can be defined, but they are (and should be) rare.
- (since C++20: `std::regular` in `<concept>`
- Handles for resources that cannot be cloned, e.g., a scoped_lock for a mutex, are concrete types but typically cannot be copied (instead, they can usually be moved), so they can't be regular; instead, they tend to be move-only.

## C.12: Don't make data members const or references
- They are not useful, and make types difficult to use by making them either uncopyable or partially uncopyable for subtle reasons.
- The const and & data members make this class "only-sort-of-copyable" -- **copy-constructible but not copy-assignable.**
```cpp
class bad {
    const int i;    // bad
    string& s;      // bad
    // ...
};
```
- Consider using `gsl::not_null` for a pointer if it should not be null, instead of a reference.