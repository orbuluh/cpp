- [C.defop: Default Operations](#cdefop-default-operations)
  - [C.20: If you can avoid defining default operations, do.](#c20-if-you-can-avoid-defining-default-operations-do)
  - [C.21: If you define or `=delete` any copy, move, or destructor function, define or `=delete` them all](#c21-if-you-define-or-delete-any-copy-move-or-destructor-function-define-or-delete-them-all)
  - [C.22: Make default operations consistent](#c22-make-default-operations-consistent)

# C.defop: Default Operations
- By default, the language supplies the default operations with their default semantics.
- However, a programmer can disable or replace these defaults.

## C.20: If you can avoid defining default operations, do.
- It's the simplest and gives the cleanest semantics.
- This is known as "the rule of zero".

## C.21: If you define or `=delete` any copy, move, or destructor function, define or `=delete` them all
- This is known as "the rule of five."
- The semantics of copy, move, and destruction are closely related, so if one needs to be declared, the odds are that others need consideration too.
  - Declaring **any copy/move/destructor** function, even as `=default` or `=delete`, **will suppress the implicit declaration of a move constructor and move assignment operator**

  - Declaring a move constructor or move assignment operator, even as `=default` or `=delete`, **will cause an implicitly generated copy constructor or implicitly generated copy assignment operator to be defined as deleted.**
- So as soon as any of these are declared, the others should all be declared to avoid unwanted effects like turning all potential moves into more expensive copies, or making a class move-only.
```cpp
struct M2 {   // bad: incomplete set of copy/move/destructor operations
public:
    // ...
    // ... no copy or move operations ...
    ~M2() { delete[] rep; }
private:
    pair<int, int>* rep;  // zero-terminated set of pairs
};
```
- Given that "special attention" was needed for the destructor (here, to deallocate), the likelihood that the implicitly-defined copy and move assignment operators will be correct is low (here, we would get double deletion).
- If you want a default implementation (while defining another), write `=default` to show you're doing so intentionally for that function. If you don't want a generated default function, suppress it with `=delete`
- When a destructor needs to be declared just to make it `virtual`, it can be defined as defaulted.
```cpp
class AbstractBase {
public:
    virtual ~AbstractBase() = default;
    // ...
};
```
- To avoid the tedium and the possibility of errors, try to follow the rule of zero.
```cpp
class X {
public:
    // ...
    virtual ~X() = default;            // destructor (virtual if X is meant to be a base class)
    X(const X&) = default;             // copy constructor
    X& operator=(const X&) = default;  // copy assignment
    X(X&&) = default;                  // move constructor
    X& operator=(X&&) = default;       // move assignment
};
```

## C.22: Make default operations consistent
- The default operations are conceptually a matched set. Their semantics are interrelated.
  - Users will be surprised if copy/move construction and copy/move assignment do logically different things.
  - Users will be surprised if constructors and destructors do not provide a consistent view of resource management.
  - Users will be surprised if copy and move don't reflect the way constructors and destructors work.
```cpp
class Bad {
    class Impl {
        // ...
    };
    shared_ptr<Impl> p;
public:
    // These operations disagree about copy semantics. This will lead to confusion and bugs.
    Bad(const Bad& a) : p(make_shared<Impl>()) { *p = *a.p; }   // deep copy
    Bad& operator=(const Bad& a) { p = a.p; }   // shallow copy
    // ...
};
```