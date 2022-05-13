# C.other: Other default operation rules
- [C.other: Other default operation rules](#cother-other-default-operation-rules)
  - [C.80: Use `=default` if you have to be explicit about using the default semantics](#c80-use-default-if-you-have-to-be-explicit-about-using-the-default-semantics)
  - [C.81: Use `=delete` when you want to disable default behavior (without wanting an alternative)](#c81-use-delete-when-you-want-to-disable-default-behavior-without-wanting-an-alternative)
  - [C.82: Don't call virtual functions in constructors and destructors](#c82-dont-call-virtual-functions-in-constructors-and-destructors)
  - [C.83: For value-like types, consider providing a `noexcept` `swap` function](#c83-for-value-like-types-consider-providing-a-noexcept-swap-function)
  - [C.84: A swap function must not fail](#c84-a-swap-function-must-not-fail)
  - [C.85: Make `swap` `noexcept`](#c85-make-swap-noexcept)
  - [C.86: Make `==` symmetric with respect to operand types and `noexcept`](#c86-make--symmetric-with-respect-to-operand-types-and-noexcept)
  - [C.87: Beware of == on base classes](#c87-beware-of--on-base-classes)
  - [C.89: Make a `hash` `noexcept`](#c89-make-a-hash-noexcept)
  - [C.90: Rely on constructors and assignment operators, not `memset` and `memcpy`](#c90-rely-on-constructors-and-assignment-operators-not-memset-and-memcpy)

## C.80: Use `=default` if you have to be explicit about using the default semantics
- The compiler is more likely to get the default semantics right and you cannot implement these functions better than the compiler.

```cpp
class Tracer {
    string message;

  public:
    Tracer(const string& m) : message{m} {
        cerr << "entering " << message << '\n';
    }
    ~Tracer() { cerr << "exiting " << message << '\n'; }

    // Because we defined the destructor, we must define the copy and move operations.
    Tracer(const Tracer&) = default;
    Tracer& operator=(const Tracer&) = default;
    Tracer(Tracer&&) = default;
    Tracer& operator=(Tracer&&) = default;
};
```

## C.81: Use `=delete` when you want to disable default behavior (without wanting an alternative)
- In a few cases, a default operation is not desirable.
- Note that deleted functions should be `public`.

```cpp
class Immortal {
  public:
    ~Immortal() = delete; // do not allow destruction
    // ...
};

void use() {
    Immortal ugh; // error: ugh cannot be destroyed
    Immortal* p = new Immortal{};
    delete p; // error: cannot destroy *p
}
```
- How `unique_ptr` achieve move-only
```cpp
template<class T, class D = default_delete<T>> class unique_ptr {
public:
    // ...
    constexpr unique_ptr() noexcept;
    explicit unique_ptr(pointer p) noexcept;
    // ...
    unique_ptr(unique_ptr&& u) noexcept;   // move constructor
    // ...
    unique_ptr(const unique_ptr&) = delete; // disable copy from lvalue
    // ...
};
```
## C.82: Don't call virtual functions in constructors and destructors
- There is nothing inherently wrong with calling virtual functions from constructors and destructors. The semantics of such calls is type safe. However, experience shows that such calls are rarely needed, easily confuse maintainers, and become a source of errors when used by novices.
- The function called will be that of **the object constructed so far**, rather than a possibly overriding function in a derived class. This can be most confusing.
- Worse, .**a direct or indirect call to an unimplemented pure virtual function from a constructor or destructor results in undefined behavior**
```cpp
class Base {
  public:
    virtual void f() = 0; // not implemented
    virtual void g();     // implemented with Base version
    virtual void h();     // implemented with Base version
    virtual ~Base();      // implemented with Base version
};

class Derived : public Base {
  public:
    void g() override; // provide Derived implementation
    void h() final;    // provide Derived implementation

    Derived() {
        // BAD: attempt to call an unimplemented virtual function
        f();

        // BAD: will call Derived::g, not dispatch further virtually
        g();

        // GOOD: explicitly state intent to call only the visible version
        Derived::g();

        // ok, no qualification needed, h is final
        h();
    }
};
```
- Note that calling a specific explicitly qualified function is not a virtual call even if the function is virtual.

## C.83: For value-like types, consider providing a `noexcept` `swap` function
- A swap can be handy for implementing a number of idioms, from smoothly moving objects around to implementing assignment easily to providing a guaranteed commit function that enables strongly error-safe calling code.
```cpp
class Foo {
  public:
    void swap(Foo& rhs) noexcept {
        m1.swap(rhs.m1);
        std::swap(m2, rhs.m2);
    }

  private:
    Bar m1;
    int m2;
};
```
- Providing a non-member `swap` function in the same namespace as your type for callers' convenience.
```cpp
void swap(Foo& a, Foo& b) { a.swap(b); }
```
- Consider using `swap` to implement copy assignment in terms of copy construction. [E.16: Destructors, deallocation, and swap must never fail](E.md#e16-destructors-deallocation-and-swap-must-never-fail)

## C.84: A swap function must not fail
- `swap` is widely used **in ways that are assumed never to fail** and programs cannot easily be written to work correctly in the presence of a failing `swap`.
- The standard-library containers and algorithms will not work correctly if a `swap` of an element type fails.

```cpp
void swap(My_vector& x, My_vector& y) {
    auto tmp = x; // copy elements
    x = y;
    y = tmp;
}
```
- Above is not just slow - if a memory allocation occurs for the elements in `tmp`, this `swap` could throw and would make STL algorithms fail if used with them.

## C.85: Make `swap` `noexcept`
- If a swap tries to exit with an exception, it's a bad design error and the program had better terminate. See [C.84](#c84-a-swap-function-must-not-fail)


## C.86: Make `==` symmetric with respect to operand types and `noexcept`
- Asymmetric treatment of operands is surprising and a source of errors where conversions are possible.
- `==` is a fundamental operation and programmers should be able to use it without fear of failure.
```cpp
class B {
    string name;
    int number;
    bool operator==(const B& a) const {
        return name == a.name && number == a.number;
    }
    // ...
};
```
- If a class has a failure state, like `double`'s `NaN`, there is a temptation to make a comparison against the failure state throw.
- The alternative is to make two failure states compare equal and any valid state compare false against the failure state.
- This rule applies to all the usual comparison operators: !=, <, <=, >, and >=.

## C.87: Beware of == on base classes
- It is really hard to write a foolproof and useful == for a hierarchy.
```cpp
class B {
    string name;
    int number;
    virtual bool operator==(const B& a) const {
        return name == a.name && number == a.number;
    }
    // ...
};

class D : B {
    char character;
    virtual bool operator==(const D& a) const {
        return name == a.name && number == a.number && character == a.character;
    }
    // ...
};

B b = ...
D d = ...
b == d; // compares name and number, ignores d's character
d == b; // error: no == defined
D d2;
d == d2; // compares name, number, and character
B& b2 = d2;
b2 == d; // compares name and number, ignores d2's and d's character
```
- Of course there are ways of making == work in a hierarchy, but the naive approaches do not scale
- This rule applies to all the usual comparison operators: !=, <, <=, >, >=, and <=>.

## C.89: Make a `hash` `noexcept`
- Users of hashed containers use hash indirectly and don't expect simple access to throw. It's a standard-library requirement.
- If you have to define a hash specialization, try simply to let it combine standard-library hash specializations with `^` (xor). That tends to work better than "cleverness" for non-specialists.

## C.90: Rely on constructors and assignment operators, not `memset` and `memcpy`
- The standard C++ mechanism to construct an instance of a type is to call its constructor.
- As specified in guideline [C.41](C.ctor.md#c41-a-constructor-should-create-a-fully-initialized-object): a constructor should create a fully initialized object. No additional initialization, such as by `memcpy`, should be required.
- A type will provide a copy constructor and/or copy assignment operator to appropriately make a copy of the class, preserving the type's invariants.
- Using `memcpy` to copy a non-trivially copyable type has undefined behavior. Frequently this results in slicing, or data corruption.
- Example, good
```cpp
struct base {
    virtual void update() = 0;
    std::shared_ptr<int> sp;
};

struct derived : public base {
    void update() override {}
};
```
- Example, bad. These are type-unsafe and overwrites the vtable.
```cpp
void init(derived& a)
{
    memset(&a, 0, sizeof(derived));
}

void copy(derived& a, derived& b)
{
    memcpy(&a, &b, sizeof(derived));
}
```