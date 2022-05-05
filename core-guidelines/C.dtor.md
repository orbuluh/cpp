# C.dtor: Destructors
- "Does this class need a destructor?" is a surprisingly insightful design question.
  - For most classes the answer is "no" either because the class holds no resources or because destruction is handled by the rule of zero; that is, its members can take care of themselves as concerns destruction.
  - If the answer is "yes", much of the design of the class follows (see the rule of five).

## C.30: Define a destructor if a class needs an explicit action at object destruction
- Only define a non-default destructor if a class needs to execute code that is not already part of its members' destructors.
- There are two general categories of classes that need a user-defined destructor:
  - A class with a resource that is not already represented as a class with a destructor, e.g., a vector or a transaction class.
  - A class that exists primarily to execute an action upon destruction, such as a tracer or final_action.

```cpp
template<typename A>
struct final_action {   // slightly simplified
    A act;
    final_action(A a) : act{a} {}
    ~final_action() { act(); }
};

template<typename A>
final_action<A> finally(A act)   // deduce action type
{
    return final_action<A>{act};
}

void test()
{
    auto act = finally([] { cout << "Exit test\n"; });  // establish exit action
    // ...
    if (something) return;   // act done here
    // ...
} // act done here
```
- If the default destructor is needed, but its generation has been suppressed (e.g., by defining a move constructor), use `=default`

## C.31: All resources acquired by a class must be released by the class's destructor
- A destructor, close, or cleanup operation should never fail. If it does nevertheless, we have a problem that has no really good solution.
  - For starters, the writer of a destructor does not know why the destructor is called and cannot "refuse to act" by throwing an exception.
  - To make the problem worse, many "close/release" operations are not retryable.
- Many have tried to solve this problem, but no general solution is known. If at all possible, **consider failure to close/cleanup a fundamental design error and terminate.**

## C.32: If a class has a raw pointer (T*) or reference (T&), consider whether it might be owning
- If the `T*` or `T&` is owning, mark it owning. If the T* is not owning, consider marking it ptr. This will aid documentation and analysis.
- Consider [T* is not owning](R.md#r3-a-raw-pointer-a-t-is-non-owning) for clarity.

## C.33: If a class has an owning pointer member, define a destructor
- An owned object must be deleted upon destruction of the object that owns it.
  - Note that [if you define a destructor, you must define or delete all default operations](C.defop.md#c21-if-you-define-or-delete-any-copy-move-or-destructor-function-define-or-delete-them-all)
- Often the simplest way to get a destructor is to replace the pointer with a smart pointer (e.g., `std::unique_ptr`) and let the compiler arrange for proper destruction to be done implicitly.

## C.35: A base class destructor should be either public and virtual, or protected and non-virtual
- To prevent undefined behavior.
- If the destructor is public, then calling code can attempt to destroy a derived class object through a base class pointer, and the result is undefined if the base class's destructor is non-virtual.
- If the destructor is protected, then calling code cannot destroy through a base class pointer and the destructor does not need to be virtual; it does need to be protected, not private, so that derived destructors can invoke it.
- In general, the writer of a base class does not know the appropriate action to be done upon destruction.
```cpp
struct Base {  // BAD: implicitly has a public non-virtual destructor
    virtual void f();
};

struct D : Base {
    string s {"a resource needing cleanup"};
    ~D() { /* ... do some cleanup ... */ }
    // ...
};

void use()
{
    unique_ptr<Base> p = make_unique<D>();
    // ...
} // p's destruction calls ~Base(), not ~D(), which leaks D::s and possibly more
```
- Note that a destructor must be non-private or it will prevent using the type
```cpp
class X {
    ~X();   // private destructor
    // ...
};

void use()
{
    X a;                        // error: cannot destroy
    auto p = make_unique<X>();  // error: cannot destroy
}
```

