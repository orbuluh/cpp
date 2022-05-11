# C.other: Other default operation rules

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