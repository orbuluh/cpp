## Discussion: Define and initialize member variables in the order of member declaration
- Member variables are always initialized in the order they are declared in the class definition, so write them in that order in the constructor initialization list.
- Writing them in a different order just makes the code confusing because it won't run in the order you see, and that can make it hard to see order-dependent bugs.

```cpp
class Employee {
    string email, first, last;
public:
    Employee(const char* firstName, const char* lastName);
    // ...
};

Employee::Employee(const char* firstName, const char* lastName)
  : first(firstName),
    last(lastName),
    // BAD: first and last not yet constructed
    email(first + "." + last + "@acme.com")
{}
```

- In this example, email will be constructed before first and last because it is declared first.
- That means its constructor will attempt to use first and last too soon -- not just before they are set to the desired values, but before they are constructed at all.
- If the class definition and the constructor body are in separate files, the long-distance influence that the order of member variable declarations has over the constructor's correctness will be even harder to spot.

# Discussion: Use of =, {}, and () as initializers
... empty

# Discussion: Use a factory function if you need "virtual behavior" during initialization
- If your design wants virtual dispatch into a derived class from a base class constructor or destructor for functions like `f` and `g`, you need other techniques, such as a post-constructor -- a separate member function the caller must invoke to complete initialization, which can safely call `f` and `g` because in member functions virtual calls behave normally.
- Some techniques for this are shown in the References.
- Here's a non-exhaustive list of options:
  - Pass the buck: Just document that user code must call the post-initialization function right after constructing an object.
  - Post-initialize lazily: Do it during the first call of a member function. A Boolean flag in the base class tells whether or not post-construction has taken place yet.
  - Use virtual base class semantics: Language rules dictate that the **constructor of the most-derived class decides which base constructor will be invoked; you can use that to your advantage.** (See [Taligent94].)
  - Use a factory function: This way, you can easily force a mandatory invocation of a post-constructor function.

- Here is an example of the last option:
```cpp
class B {
  public:
    B() {
        /* ... */
        f(); // BAD: C.82: Don't call virtual functions in constructors and
             // destructors
        /* ... */
    }

    virtual void f() = 0;
};
```
```cpp
class B {
  protected:
    class Token {};

  public:
    // constructor needs to be public so that make_shared can access it.
    // protected access level is gained by requiring a Token.
    explicit B(Token) { /* ... */
    }                   // create an imperfectly initialized object
    virtual void f() = 0;

    template <class T>
    static shared_ptr<T> create() // interface for creating shared objects
    {
        auto p = make_shared<T>(typename T::Token{});
        p->post_initialize();
        return p;
    }

  protected:
    virtual void post_initialize() // called right after construction
    {                              /* ... */
        f();                       /* ... */
    }                              // GOOD: virtual dispatch is safe
};

class D : public B { // some derived class
  protected:
    class Token {};

  public:
    // constructor needs to be public so that make_shared can access it.
    // protected access level is gained by requiring a Token.
    explicit D(Token) : B{B::Token{}} {}
    void f() override{/* ...  */};

  protected:
    template <class T> friend shared_ptr<T> B::create();
};

shared_ptr<D> p = D::create<D>(); // creating a D object
```
- This design requires the following discipline:
  - Derived classes such as D must not expose a publicly callable constructor. Otherwise, D's users could create D objects that don't invoke `post_initialize`.
  - Allocation is limited to operator new.
  - B can, however, override new (see Items 45 and 46 in SuttAlex05).
  - D must define a constructor with the same parameters that B selected.
  - Defining several overloads of create can assuage this problem, however; and the overloads can even be templated on the argument types.
- If the requirements above are met, the design guarantees that `post_initialize` has been called for any fully constructed B-derived object.
- `post_initialize` doesn't need to be virtual; it can, however, invoke virtual functions freely.

- In summary, no post-construction technique is perfect.
  - The worst techniques dodge the whole issue by simply asking the caller to invoke the post-constructor manually.
  - Even the best require a different syntax for constructing objects (easy to check at compile time) and/or cooperation from derived class authors (impossible to check at compile time).

# Discussion: Usage of `noexcept`
empty

# Discussion: Destructors, deallocation, and swap must never fail
- Never allow an error to be reported from a destructor, a resource deallocation function (e.g., operator `delete`), or a `swap` function using `throw`.
- It is nearly impossible to write useful code if these operations can fail, and even if something does go wrong it nearly never makes any sense to retry.
- Specifically, types whose destructors might throw an exception are flatly forbidden from use with the C++ Standard Library. Most destructors are now implicitly `noexcept` by default.

```cpp
class Nefarious {
public:
    Nefarious() { /* code that could throw */ }    // ok
    ~Nefarious() { /* code that could throw */ }   // BAD, should not throw
    // ...
};
```
- `Nefarious` objects are hard to use safely even as local variables:
```cpp
 void test(string& s)
 {
     Nefarious n;          // trouble brewing
     string copy = s;      // copy the string
 } // destroy copy and then n
 ```
- Here, copying s could throw, and if that throws and if n's destructor then also throws, the program will exit via `std::terminate` because **two exceptions can't be propagated simultaneously.**
- Classes with `Nefarious` members or bases are also hard to use safely, because their destructors must invoke `Nefarious`' destructor, and are similarly poisoned by its bad behavior:

```cpp
 class Innocent_bystander {
     Nefarious member;     // oops, poisons the enclosing class's destructor
     // ...
 };

 void test(string& s)
 {
     Innocent_bystander i;  // more trouble brewing
     string copy2 = s;      // copy the string
 } // destroy copy and then i
 ```
- Here, if constructing copy2 throws, we have the same problem because i's destructor now also can throw, and if so we'll invoke `std::terminate`.
- You can't reliably create global or static `Nefarious` objects either:
```cpp
static Nefarious n;       // oops, any destructor exception can't be caught
```

- You can't reliably create arrays of `Nefarious`:
```cpp
 void test()
 {
     std::array<Nefarious, 10> arr; // this line can std::terminate()
 }
 ```
- **The behavior of arrays is undefined in the presence of destructors that throw because there is no reasonable rollback behavior that could ever be devised.**
  - Just think: What code can the compiler generate for constructing an arr where, if the fourth object's constructor throws, the code has to give up and in its cleanup mode tries to call the destructors of the already-constructed objects ... and one or more of those destructors throws? There is no satisfactory answer.

- You can't use `Nefarious` objects in standard containers:
```cpp
std::vector<Nefarious> vec(10);   // this line can std::terminate()
```
- The standard library forbids all destructors used with it from throwing. You can't store `Nefarious` objects in standard containers or use them with any other part of the standard library.

Note: These are key functions that must not fail because they are necessary for the two key operations in transactional programming:
- to back out work if problems are encountered during processing, and to commit work if no problems occur.
- **If there's no way to safely back out using no-fail operations, then no-fail rollback is impossible to implement.**
- **If there's no way to safely commit state changes using a no-fail operation (notably, but not limited to, `swap`), then no-fail commit is impossible to implement.**

- Consider the following advice and requirements found in the C++ Standard:
  - If a destructor called during stack unwinding exits with an exception, `terminate` is called (15.5.1). So **destructors should generally catch exceptions and not let them propagate out of the destructor**. --[C++03] §15.2(3)
  - No destructor operation defined in the C++ Standard Library (including the destructor of any type that is used to instantiate a standard-library template) will throw an exception. --[C++03] §17.4.4.8(3)

- Deallocation functions, including specifically overloaded operator `delete` and operator `delete[]`, fall into the same category, because they too are used during cleanup in general, and during exception handling in particular, to back out of partial work that needs to be undone.
- Besides destructors and deallocation functions, common error-safety techniques rely also on `swap` operations never failing -- in this case, not because they are used to implement a **guaranteed rollback**, but because they are used to implement a **guaranteed commit**.
- For example, here is an idiomatic implementation of `operator=` for a type `T` that performs copy construction followed by a call to a no-fail `swap`:

```cpp
T& T::operator=(const T& other)
{
    auto temp = other;
    swap(temp);
    return *this;
}
```

- Fortunately, when releasing a resource, the scope for failure is definitely smaller. If using exceptions as the error reporting mechanism, make sure such functions handle all exceptions and other errors that their internal processing might generate.
- (For exceptions, simply wrap everything sensitive that your destructor does in a `try/catch(...)` block.)
- This is particularly important because a destructor might be called in a crisis situation, such as failure to allocate a system resource (e.g., memory, files, locks, ports, windows, or other system objects).
- **When using exceptions as your error handling mechanism, always document this behavior by declaring these functions `noexcept`**. (See Item 75.)
