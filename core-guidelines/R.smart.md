# R.smart: Smart pointers
- [R.smart: Smart pointers](#rsmart-smart-pointers)
  - [R.20: Use `unique_ptr` or `shared_ptr` to represent ownership](#r20-use-unique_ptr-or-shared_ptr-to-represent-ownership)
  - [R.21: Prefer `unique_ptr` over `shared_ptr` unless you need to share ownership](#r21-prefer-unique_ptr-over-shared_ptr-unless-you-need-to-share-ownership)
  - [R.22: Use `make_shared()` to make `shared_ptr`s](#r22-use-make_shared-to-make-shared_ptrs)
  - [R.23: Use `make_unique()` to make `unique_ptr`s](#r23-use-make_unique-to-make-unique_ptrs)
  - [R.24: Use `std::weak_ptr` to break cycles of `std::shared_ptrs`](#r24-use-stdweak_ptr-to-break-cycles-of-stdshared_ptrs)
  - [R.30: Take smart pointers as parameters only to explicitly express lifetime semantics](#r30-take-smart-pointers-as-parameters-only-to-explicitly-express-lifetime-semantics)
  - [R.31: If you have non-std smart pointers, follow the basic pattern from std](#r31-if-you-have-non-std-smart-pointers-follow-the-basic-pattern-from-std)
  - [R.32: Take a `unique_ptr<widget>` parameter to express that **a function assumes ownership of a widget**](#r32-take-a-unique_ptrwidget-parameter-to-express-that-a-function-assumes-ownership-of-a-widget)
  - [R.33: Take a `unique_ptr<widget>&` parameter to express that a function reseats the `widget`](#r33-take-a-unique_ptrwidget-parameter-to-express-that-a-function-reseats-the-widget)
  - [R.34: Take a `shared_ptr<widget>` parameter (by value) to express shared ownership](#r34-take-a-shared_ptrwidget-parameter-by-value-to-express-shared-ownership)
  - [R.35: Take a `shared_ptr<widget>&` parameter (by reference) to express that **a function might reseat the shared pointer**](#r35-take-a-shared_ptrwidget-parameter-by-reference-to-express-that-a-function-might-reseat-the-shared-pointer)
  - [R.36: Take a `const shared_ptr<widget>&` parameter to express that **it might retain a reference count to the object**](#r36-take-a-const-shared_ptrwidget-parameter-to-express-that-it-might-retain-a-reference-count-to-the-object)
  - [R.37: Do not pass a pointer or reference obtained from an aliased smart pointerReason](#r37-do-not-pass-a-pointer-or-reference-obtained-from-an-aliased-smart-pointerreason)

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

## R.21: Prefer `unique_ptr` over `shared_ptr` unless you need to share ownership
- A `unique_ptr` is conceptually simpler and more predictable (you know when destruction happens) and faster (you don't implicitly maintain a use count).

## R.22: Use `make_shared()` to make `shared_ptr`s
- `make_shared` gives a more concise statement of the construction. It also gives an opportunity to** eliminate a separate allocation for the reference counts**, by placing the shared_ptr's use counts next to its object.
```cpp
shared_ptr<X> p1{new X{2}}; // bad
auto p = make_shared<X>(2); // good
```

## R.23: Use `make_unique()` to make `unique_ptr`s
- `make_unique` gives a more concise statement of the construction. It also ensures exception safety in complex expressions.

## R.24: Use `std::weak_ptr` to break cycles of `std::shared_ptrs`
- `shared_ptr`'s rely on use counting and the **use count for a cyclic structure never goes to zer**o, so we need a mechanism to be able to destroy a cyclic structure.
  - As breaking cycles is what you must do; temporarily sharing ownership (through `weak_pointer`) is how you do it.
  - (e.g. You could "temporarily share ownership" simply by using another `shared_ptr` - but it won't break the cycles)

```cpp
#include <memory>

class bar;

class foo {
  public:
    explicit foo(const std::shared_ptr<bar>& forward_reference)
        : forward_reference_(forward_reference) {}

  private:
    std::shared_ptr<bar> forward_reference_;
};

class bar {
  public:
    explicit bar(const std::weak_ptr<foo>& back_reference)
        : back_reference_(back_reference) {}
    void do_something() {
        if (auto shared_back_reference = back_reference_.lock()) {
            // Use *shared_back_reference
        }
    }

  private:
    std::weak_ptr<foo> back_reference_;
};
```

## R.30: Take smart pointers as parameters only to explicitly express lifetime semantics
- [F.7](F.md)

## R.31: If you have non-std smart pointers, follow the basic pattern from std
- Any type (including primary template or specialization) that overloads unary `*` and `->` is considered a smart pointer:
- If it is copyable, it is recognized as a reference-counted `shared_ptr`.
- If it is not copyable, it is recognized as a unique `unique_ptr`.

```cpp
// use Boost's intrusive_ptr
#include <boost/intrusive_ptr.hpp>
void f(boost::intrusive_ptr<widget> p) // error under rule 'R.30'
{
    p->foo();
}

// use Microsoft's CComPtr
#include <atlbase.h>
void f(CComPtr<widget> p) // error under rule 'R.30'
{
    p->foo();
}
```
- Both cases are an error under the [R.30](#r30-take-smart-pointers-as-parameters-only-to-explicitly-express-lifetime-semantics) guideline: p is a Shared_pointer, but nothing about its sharedness is used here and passing it by value is a silent pessimization;
  - **these functions should accept a smart pointer only if they need to participate in the widget's lifetime management.**
  - Otherwise they should accept a `widget*`, if it can be `nullptr`.
  - O**therwise, and ideally, the function should accept a `widget&`**.
- These smart pointers match the Shared_pointer concept, so these guideline enforcement rules work on them out of the box and expose this common pessimization.

## R.32: Take a `unique_ptr<widget>` parameter to express that **a function assumes ownership of a widget**
- Using `unique_ptr` in this way both **documents and enforces the function call's ownership transfer**.

```cpp
// bad
void uses(const unique_ptr<widget>&); // usually not what you want

// prefer
void sink(unique_ptr<widget>); // takes ownership of the widget
void uses(widget*);            // just uses the widget
```

## R.33: Take a `unique_ptr<widget>&` parameter to express that a function reseats the `widget`
- Using `unique_ptr` in this way both documents and enforces the function call's reseating semantics.
- "reseat" means "making a pointer or a smart pointer refer to a different object."

```cpp
void reseat(unique_ptr<widget>&); // "will" or "might" reseat pointer

Example, bad
void thinko(const unique_ptr<widget>&); // usually not what you want
```

## R.34: Take a `shared_ptr<widget>` parameter (by value) to express shared ownership
- This makes the function's ownership sharing explicit.

```cpp
// Example, good
class WidgetUser {
  public:
    // WidgetUser will share ownership of the widget
    explicit WidgetUser(std::shared_ptr<widget> w) noexcept
        : m_widget{std::move(w)} {}
    // ...
  private:
    std::shared_ptr<widget> m_widget;
};
```

## R.35: Take a `shared_ptr<widget>&` parameter (by reference) to express that **a function might reseat the shared pointer**
- This makes the function's reseating explicit.
- "reseat" means "making a reference or a smart pointer refer to a different object."

```cpp
// Example, good
void ChangeWidget(std::shared_ptr<widget>& w) {
    // This will change the callers widget
    w = std::make_shared<widget>(widget{});
}
```

## R.36: Take a `const shared_ptr<widget>&` parameter to express that **it might retain a reference count to the object**

- This makes the function's interface(?) explicit.

```cpp
// Example, good
void share(shared_ptr<widget>);            // share -- "will" retain refcount
void reseat(shared_ptr<widget>&);          // "might" reseat ptr
void may_share(const shared_ptr<widget>&); // "might" retain refcount
```

## R.37: Do not pass a pointer or reference obtained from an aliased smart pointerReason
- Violating this rule is the number one cause of losing reference counts and finding yourself with a dangling pointer.
- **Functions should prefer to pass raw pointers and references down call chains**.
- At the top of the call tree where you obtain the raw pointer or reference from a smart pointer that keeps the object alive. You need to be sure that the smart pointer cannot inadvertently be reset or reassigned from within the call tree below.
- To do this, sometimes you need to take a local copy of a smart pointer, which firmly keeps the object alive for the duration of the function and the call tree.
- Consider example ...
```cpp
// global (static or heap), or aliased local ...
shared_ptr<widget> g_p = ...;

void f(widget& w) {
    g();
    use(w); // A
}

void g() {
    g_p = ...; // oops, if this was the last shared_ptr to that widget, destroys
               // the widget
}
```
- The following should not pass code review :
```cpp
void my_code() {
    // BAD: passing pointer or reference obtained from a non-local smart pointer
    //      that could be inadvertently reset somewhere inside f or its callees
    f(*g_p);

    // BAD: same reason, just passing it as a "this" pointer
    g_p->func();
}
```
- The fix is simple -- take a local copy of the pointer to "keep a ref count" for your call tree:
```cpp
void my_code() {
    // cheap: 1 increment covers this entire function and all the call trees
    // below us
    auto pin = g_p;

    // GOOD: passing pointer or reference obtained from a local unaliased smart
    // pointer
    f(*pin);

    // GOOD: same reason
    pin->func();
}
```