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
