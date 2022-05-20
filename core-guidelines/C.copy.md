# C.copy: Copy and move
- Concrete types should generally be copyable, but interfaces in a class hierarchy should not.
- Resource handles might or might not be copyable. Types can be defined to move for logical as well as performance reasons.

- [C.copy: Copy and move](#ccopy-copy-and-move)
  - [C.60: Make copy assignment non-virtual, take the parameter by `const&`, and return by `non-const&`](#c60-make-copy-assignment-non-virtual-take-the-parameter-by-const-and-return-by-non-const)
  - [C.61: A copy operation should copy](#c61-a-copy-operation-should-copy)
  - [C.62: Make copy assignment safe for self-assignment](#c62-make-copy-assignment-safe-for-self-assignment)
  - [C.63: Make move assignment non-virtual, take the parameter by `&&`, and return by `non-const&`](#c63-make-move-assignment-non-virtual-take-the-parameter-by--and-return-by-non-const)
  - [C.64: A `move` operation should move and leave its source in a valid state](#c64-a-move-operation-should-move-and-leave-its-source-in-a-valid-state)
  - [C.65: Make move assignment safe for self-assignment](#c65-make-move-assignment-safe-for-self-assignment)
  - [C.66: Make move operations `noexcept`](#c66-make-move-operations-noexcept)
  - [C.67 A polymorphic class should suppress public copy/move](#c67-a-polymorphic-class-should-suppress-public-copymove)


## C.60: Make copy assignment non-virtual, take the parameter by `const&`, and return by `non-const&`
```cpp
class Foo {
public:
    Foo& operator=(const Foo& x)
    {
        // GOOD: no need to check for self-assignment (other than performance)
        auto tmp = x; // if expensive, check next snippet
        swap(tmp); // see C.83 - The swap implementation technique offers the strong guarantee.
        return *this;
    }
    // ...
};

Foo a;
Foo b;
Foo f();

a = b;    // assign lvalue: copy
a = f();  // assign rvalue: potentially move
```
- But what if you can get significantly better performance by not making a temporary copy? In this case, the copy of elements implied by the swap implementation technique could cause an order of magnitude increase in cost:
```cpp
//??? example in doc not clear to me
```
-  If you think you need a virtual assignment operator, and understand why that's deeply problematic, don't call it `operator=`. Make it a named function like `virtual void assign(const Foo&)`. See [copy constructor vs. clone()](#c67-a-polymorphic-class-should-suppress-public-copymove).

## C.61: A copy operation should copy
- That is the generally assumed semantics. After `x = y`, we should have `x == y` After a copy, x and y can ...
  - 1. be independent objects (value semantics, the way non-pointer built-in types and the standard-library types work) or
  - 2. refer to a shared object (pointer semantics, the way pointers work).
- Prefer value semantics unless you are building a "smart pointer". Value semantics is the simplest to reason about and what the standard-library facilities expect.

```cpp
class X { // OK: value semantics
  public:
    X();
    X(const X&);   // copy X
    void modify(); // change the value of X
    // ...
    ~X() { delete[] p; }

  private:
    T* p;
    int sz;
};

bool operator==(const X& a, const X& b) {
    return a.sz == b.sz && equal(a.p, a.p + a.sz, b.p, b.p + b.sz);
}

X::X(const X& a) : p{new T[a.sz]}, sz{a.sz} { copy(a.p, a.p + sz, p); }

X x;
X y = x;
if (x != y)
    throw Bad{};
x.modify();
if (x == y)
    throw Bad{}; // assume value semantics
```
```cpp
class PtrWrapper { // OK: pointer semantics
  public:
    PtrWrapper();
    PtrWrapper(const PtrWrapper&) = default; // shallow copy
    ~PtrWrapper() = default;
    void modify(); // change the pointed-to value
    // ...
  private:
    T* p;
    int sz;
};

bool operator==(const PtrWrapper& a, const PtrWrapper& b) { return a.sz == b.sz && a.p == b.p; }

PtrWrapper x;
PtrWrapper y = x;
if (x != y)
    throw Bad{};
x.modify();
if (x != y)
    throw Bad{}; // assume pointer semantics
```

## C.62: Make copy assignment safe for self-assignment
- If `x = x` changes the value of x, people will be surprised and bad errors will occur (often including leaks).
- Assignment operators should not contain the pattern `if (this == &a) return *this;`, often it is faster and more elegant to cope without such a test (e.g., using `swap`).
```cpp
class Bad {
    string s;
    int i;
public:
    Bad& operator=(const Bad& a);
    // ...
};

Bad& Bad::operator=(const Bad& a)   // OK, but there is a cost
{
    if (this == &a) return *this; //  what if we do one self-assignment per million assignments?
    s = a.s;
    i = a.i;
    return *this;
```
- Consider:
```cpp
Foo& Foo::operator=(const Foo& a)   // simpler, and probably much better
{
    // std::string is safe for self-assignment and so are int.
    // All the cost is carried by the (rare) case of self-assignment.
    s = a.s;
    i = a.i;
    return *this;
}
```
## C.63: Make move assignment non-virtual, take the parameter by `&&`, and return by `non-const&`
- Equivalent to what is done for [copy-assignment](#c60-make-copy-assignment-non-virtual-take-the-parameter-by-const-and-return-by-non-const).


## C.64: A `move` operation should move and leave its source in a valid state
- That is the generally assumed semantics. After `y = std::move(x)` the value of `y` should be the value `x` had and `x` should be in a valid state.
- e.g. Unless there is an exceptionally strong reason not to, make `x = std::move(y); y = z;` work with the conventional semantics.
```cpp
template <typename T> class X { // OK: value semantics
  public:
    X();
    X(X&& a) noexcept; // move X
    void modify();     // change the value of X
    // ...
    ~X() { delete[] p; }

  private:
    T* p;
    int sz;
};

X::X(X&& a)
    : p{a.p}, sz{a.sz} // steal representation
{
    a.p = nullptr; // set to "empty"
    a.sz = 0;
}

void use() {
    X x{};
    // ...
    X y = std::move(x);
    x = X{}; // OK
} // OK: x can be destroyed
```
- Ideally, that moved-from should be the default value of the type. Ensure that unless there is an exceptionally good reason not to.
- However, not all types have a default value and for some types establishing the default value can be expensive.
- **The standard requires only that the moved-from object can be destroyed.**
- Often, we can easily and cheaply do better: **The standard library assumes that it is possible to assign to a moved-from object.** Always leave the moved-from object in some **(necessarily specified) valid state.**

## C.65: Make move assignment safe for self-assignment
- If `x = x` changes the value of x, people will be surprised and bad errors can occur. However, people don't usually directly write a self-assignment that turn into a `move`, but it can occur.
- However, `std::swap` is implemented using `move` operations so if you accidentally do `swap(a, b)` where a and b refer to the same object, failing to handle self-move could be a serious and subtle error.
- The one-in-a-million argument against `if (this == &a) return *this;` tests from the discussion of self-assignment is even more relevant for self-move.
```cpp
class Foo {
    string s;
    int i;

  public:
    Foo& operator=(Foo&& a);
    // ...
};

Foo& Foo::operator=(Foo&& a) noexcept // OK, but there is a cost
{
    if (this == &a)
        return *this; // this line is redundant
    s = std::move(a.s);
    i = a.i;
    return *this;
}
```
- There is no known general way of avoiding an `if (this == &a) return *this; `test for a move assignment and still get a correct answer (i.e., after x = x the value of x is unchanged).
- The ISO standard guarantees only a "valid but unspecified" state for the standard-library containers. The rule here is more caution and insists on complete safety.

## C.66: Make move operations `noexcept`
- A throwing move violates most people's reasonable assumptions. A non-throwing move will be used more efficiently by standard-library and language facilities.
```cpp
template <typename T> class Vector {
  public:
    Vector(Vector&& a) noexcept : elem{a.elem}, sz{a.sz} {
        a.sz = 0;
        a.elem = nullptr;
    }
    Vector& operator=(Vector&& a) noexcept {
        elem = a.elem;
        sz = a.sz;
        a.sz = 0;
        a.elem = nullptr; // make a back to a valid state #65
    }
    // ...
  private:
    T* elem;
    int sz;
};
```
```cpp
template <typename T> class Bad {
  public:
    // Not just inefficient, but since a vector copy requires allocation, it can throw!!!
    Bad(Bad&& a) { *this = a; }            // just use the copy
    Bad& operator=(Bad&& a) { *this = a; } // just use the copy
    // ...
  private:
    T* elem;
    int sz;
};
```
## C.67 A polymorphic class should suppress public copy/move
- A polymorphic class is a class that defines or inherits at least one virtual function.
- It is likely that it will be used as a base class for other derived classes with polymorphic behavior.
- If it is accidentally passed by value, **with the implicitly generated copy constructor and assignment, we risk slicing:** only the base portion of a derived object will be copied, and the polymorphic behavior will be corrupted.
- If the class has no data, `=delete` the copy/move functions. Otherwise, make them `protected`.
- Also check [C.145: Access polymorphic objects through pointers and references](C.hier.access.md#c145-access-polymorphic-objects-through-pointers-and-references)

```cpp
class B { // BAD: polymorphic base class doesn't suppress copying
  public:
    virtual char m() { return 'B'; }
    // ... nothing about copy operations, so uses default ...
};

class D : public B {
  public:
    char m() override { return 'D'; }
    // ...
};

void f(B& b) {
    auto b2 = b; // oops, slices the object; b2.m() will return 'B'
}

D d;
f(d);
```
```cpp
class B { // GOOD: polymorphic class suppresses copying
  public:
    B() = default;
    B(const B&) = delete;
    B& operator=(const B&) = delete;
    virtual char m() { return 'B'; }
    // ...
};

class D : public B {
  public:
    char m() override { return 'D'; }
    // ...
};

void f(B& b) {
    auto b2 = b; // ok, compiler will detect inadvertent copying, and protest
}

D d;
f(d);
```
- If you need to create deep copies of polymorphic objects, use clone() functions: see [C.130](C.hierclass.md#c130-for-making-deep-copies-of-polymorphic-classes-prefer-a-virtual-clone-function-instead-of-public-copy-constructionassignment).
- Exceptions: Classes that represent exception objects need both to be polymorphic and copy-constructible.
