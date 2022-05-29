# R: Resource management
- **A resource is anything that must be acquired and (explicitly or implicitly) released**, such as memory, file handles, sockets, and locks.
- The reason it must be released is typically that **it can be in short supply**, so even delayed release might do harm.
- The fundamental aim is to ensure that we don't leak any resources and that we don't hold a resource longer than we need to.
- An entity that is responsible for releasing a resource is called an owner.

- There are a few cases where leaks can be acceptable or even optimal:
  - If you are writing a program that simply produces an output based on an input and the amount of memory needed is proportional to the size of the input, the optimal strategy (for performance and ease of programming) is sometimes simply never to delete anything.
  - If you have enough memory to handle your largest input, leak away, but be sure to give a good error message if you are wrong. Here, we ignore such cases.

## R.1: Manage resources automatically using resource handles and RAII (Resource Acquisition Is Initialization)
- To avoid leaks and the complexity of manual resource management. C++'s language-enforced constructor/destructor symmetry mirrors the symmetry inherent in resource acquire/release function pairs such as fopen/fclose, lock/unlock, and new/delete.
- Whenever you deal with a resource that needs paired acquire/release function calls, encapsulate that resource in an object that enforces pairing for you -- acquire the resource in its constructor, and release it in its destructor.

```cpp
// bad
void send(X* x, string_view destination) {
    auto port = open_port(destination);
    my_mutex.lock();
    // ... if any exception thrown, resource leaks..
    send(port, x);
    // ... if any exception thrown, resource leaks..
    my_mutex.unlock();
    close_port(port);
    delete x;
}
```
```cpp
class Port {
    PortHandle port;

  public:
    Port(string_view destination) : port{open_port(destination)} {}
    ~Port() { close_port(port); }
    operator PortHandle() { return port; }

    // port handles can't usually be cloned, so disable copying and assignment
    // if necessary
    Port(const Port&) = delete;
    Port& operator=(const Port&) = delete;
};

void send(unique_ptr<X> x, string_view destination) // x owns the X
{
    Port port{destination};            // port owns the PortHandle
    lock_guard<mutex> guard{my_mutex}; // guard owns the lock
    // ...
    send(port, x);
    // ...
} // automatically unlocks my_mutex and deletes the pointer in x
```

## R.2: In interfaces, use raw pointers to denote individual objects (only)
- Arrays are best represented by a container type (e.g., vector (owning)) or a span (non-owning). Such containers and views hold sufficient information to do range checking.
```cpp
void f(int* p, int n) // n is the number of elements in p[]
{
    // ...
    p[2] = 7; // bad: subscript raw pointer
    // ...
}
```
- The compiler does not read comments, and without reading other code you do not know whether p really points to n elements. Use a `span` instead.
- Exception: C-style strings are passed as single pointers to a zero-terminated sequence of characters. Use `zstring` rather than `char*` to indicate that you rely on that convention.

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


## R.4: A raw reference (a `T&`) is non-owning
- There is nothing (in the C++ standard or in most code) to say otherwise and most raw references are non-owning.
- We want owners identified so that we can reliably and efficiently delete the objects pointed to by owning pointers.
- [R.3](#r3-a-raw-pointer-a-t-is-non-owning)

## R.5: Prefer scoped objects, don't heap-allocate unnecessarily
- A scoped object is a local object, a global object, or a member.
- This implies that there is **no separate allocation and deallocation cost in excess of that already used for the containing scope or object.** 
- The members of a scoped object are themselves scoped and the scoped object's constructor and destructor manage the members' lifetimes.

## R.6: Avoid non-const global variables
- [I.2](I.md#i2-avoid-non-const-global-variables)

# Subsections
- [R.alloc](R.alloc.md)
- [R.smart](R.smart.md)