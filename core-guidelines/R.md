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

## R.20: Use `unique_ptr` or `shared_ptr` to represent ownership
- Consider:
```cpp
void f()
{
    X x;
    X* p1 { new X };              // will leak!
    std::unique_ptr<X> p2 { new X };   // unique ownership; see also ???
    std::shared_ptr<X> p3 { new X };   // shared ownership; see also ???
    auto p4 = std::make_unique<X>();   // unique_ownership, preferable to the explicit use "new"
    auto p5 = std::make_shared<X>();   // shared ownership, preferable to the explicit use "new"
}
```