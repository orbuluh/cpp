# C.copy: Copy and move
- Concrete types should generally be copyable, but interfaces in a class hierarchy should not.
- Resource handles might or might not be copyable. Types can be defined to move for logical as well as performance reasons.


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



## C.67 A polymorphic class should suppress public copy/move
