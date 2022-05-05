# R: Resource management

## R.3: A raw pointer (a `T*`) is non-owning
- There is nothing (in the C++ standard or in most code) to say otherwise and most raw pointers are non-owning. We want owning pointers identified so that we can reliably and efficiently delete the objects pointed to by owning pointers.
- The `unique_ptr<T>` protects against leaks by guaranteeing the deletion of its object (even in the presence of exceptions). The `T*` does not.
```cpp
template<typename T>
class X {
public:
    T* p;   // bad: it is unclear whether p is owning or not
    T* q;   // bad: it is unclear whether q is owning or not
    // ...
};

template<typename T>
class X2 {
public:
    owner<T*> p;  // OK: p is owning
    T* q;         // OK: q is not owning
    // ...
};
```
- If pointer semantics are required (e.g., because the return type needs to refer to a base class of a class hierarchy (an interface)), return a "smart pointer."
- 