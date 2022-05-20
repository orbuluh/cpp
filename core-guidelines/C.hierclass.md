# C.hierclass: Designing classes in a hierarchy

- [C.hierclass: Designing classes in a hierarchy](#chierclass-designing-classes-in-a-hierarchy)
  - [C.126: An abstract class typically doesn't need a user-written constructor](#c126-an-abstract-class-typically-doesnt-need-a-user-written-constructor)
  - [C.127: A class with a virtual function should have a virtual or protected destructor](#c127-a-class-with-a-virtual-function-should-have-a-virtual-or-protected-destructor)
  - [C.128: Virtual functions should specify exactly one of `virtual`, `override`, or `final`](#c128-virtual-functions-should-specify-exactly-one-of-virtual-override-or-final)
  - [C.129: When designing a class hierarchy, distinguish between **implementation inheritance** and **interface inheritance**](#c129-when-designing-a-class-hierarchy-distinguish-between-implementation-inheritance-and-interface-inheritance)
  - [C.130: For making deep copies of polymorphic classes prefer a virtual clone function instead of public copy construction/assignment](#c130-for-making-deep-copies-of-polymorphic-classes-prefer-a-virtual-clone-function-instead-of-public-copy-constructionassignment)
  - [C.131: Avoid trivial getters and setters](#c131-avoid-trivial-getters-and-setters)
  - [C.132: Don't make a function virtual without reason](#c132-dont-make-a-function-virtual-without-reason)
  - [C.133: Avoid protected data](#c133-avoid-protected-data)
  - [C.134: Ensure all non-const data members have the same access level](#c134-ensure-all-non-const-data-members-have-the-same-access-level)
  - [C.135: Use multiple inheritance to represent multiple distinct interfaces](#c135-use-multiple-inheritance-to-represent-multiple-distinct-interfaces)
  - [C.136: Use multiple inheritance to represent the union of implementation attributes](#c136-use-multiple-inheritance-to-represent-the-union-of-implementation-attributes)
  - [C.137: Use virtual bases to avoid overly general base classes](#c137-use-virtual-bases-to-avoid-overly-general-base-classes)
  - [C.138: Create an overload set for a derived class and its bases with `using`](#c138-create-an-overload-set-for-a-derived-class-and-its-bases-with-using)
  - [C.139: Use `final` on classes sparingly](#c139-use-final-on-classes-sparingly)
  - [C.140: Do not provide different default arguments for a virtual function and an overrider](#c140-do-not-provide-different-default-arguments-for-a-virtual-function-and-an-overrider)

## C.126: An abstract class typically doesn't need a user-written constructor
- An abstract class typically does not have any data for a constructor to initialize.
```cpp
class Shape {
  public:
    // no user-written constructor needed in abstract base class
    virtual Point center() const = 0; // pure virtual
    virtual void move(Point to) = 0;
    // ... more pure virtual functions...
    virtual ~Shape() {} // destructor
};

class Circle : public Shape {
  public:
    Circle(Point p, int rad); // constructor in derived class
    Point center() const override { return x; }
};
```
- In extremely rare cases, you might find it reasonable for an abstract class to have a bit of data shared by all derived classes (e.g., use statistics data, debug information, etc.); such classes tend to have constructors.
  - But be warned: Such classes also tend to be prone to requiring virtual inheritance.

## C.127: A class with a virtual function should have a virtual or protected destructor
- A class with a virtual function is usually (and in general) used via a pointer to base. Usually, the last user has to call delete on a pointer to base, often via a smart pointer to base, so the destructor should be public and virtual.
- Less commonly, if deletion through a pointer to base is not intended to be supported, the destructor should be protected and non-virtual; see [C.35](C.dtor.md#c35-a-base-class-destructor-should-be-either-public-and-virtual-or-protected-and-non-virtual).
```cpp
struct B {
    virtual int f() = 0;
    // ... no user-written destructor, defaults to public non-virtual ...
};

// bad: derived from a class without a virtual destructor
struct D : B {
    string s{"default"};
    // ...
};

void use() {
    unique_ptr<B> p = make_unique<D>();
    // ...
} // undefined behavior, might call B::~B only and leak the string
```

## C.128: Virtual functions should specify exactly one of `virtual`, `override`, or `final`
- Readability. Detection of mistakes. Writing explicit `virtual`, `override`, or `final` is self-documenting and enables the compiler to catch mismatch of types and/or names between base and derived classes.
  - `virtual` means **exactly and only "this is a new virtual function."**
  - `override` means **exactly and only "this is a non-final overrider."**
  - `final` means **exactly and only "this is a final overrider."**
- However, writing more than one of these three is both redundant and a potential source of errors.

```cpp
struct B {
    void f1(int);
    virtual void f2(int) const;
    virtual void f3(int);
    // ...
};

struct D : B {
    void f1(int); // bad (hope for a warning): D::f1() hides B::f1()
    void
    f2(int) const;   // bad (but conventional and valid): no explicit override
    void f3(double); // bad (hope for a warning): D::f3() hides B::f3()
    // ...
};
```
```cpp
struct Better : B {
    void f1(int) override; // error (caught): Better::f1() hides B::f1()
    void f2(int) const override;
    void f3(double) override; // error (caught): Better::f3() hides B::f3()
    // ...
};
```
- Note: Use `final` on functions sparingly. **It does not necessarily lead to optimization, and it precludes further overriding.**

## C.129: When designing a class hierarchy, distinguish between **implementation inheritance** and **interface inheritance**
- **Implementation details in an interface make the interface brittle; that is, make its users vulnerable to having to recompile after changes in the implementation.**
- **Data in a base class increases the complexity of implementing the base and can lead to replication of code.**
- Definition:
  - **interface inheritance** is the use of inheritance to separate users from implementations, in particular to allow derived classes to be added and changed without affecting the users of base classes.
  - **implementation inheritance** is the use of inheritance to simplify implementation of new facilities by making useful operations available for implementers of related new operations (sometimes called **"programming by difference"**).
- A pure interface class is simply a set of pure virtual functions; see [I.25](I.md#i25-prefer-empty-abstract-classes-as-interfaces-to-class-hierarchies).
- The importance of keeping the two kinds of inheritance increases
  - with the size of a hierarchy (e.g., dozens of derived classes),
  - with the length of time the hierarchy is used (e.g., decades), and
  - with the number of distinct organizations in which a hierarchy is used (e.g., it can be difficult to distribute an update to a base class)
- Bad example:
  - Problems:
    - As the hierarchy grows and more data is added to Shape, the constructors get harder to write and maintain.
    - Why calculate the center for the Triangle? we might never use it.
    - Add a data member to Shape (e.g., drawing style or canvas) and all classes derived from Shape and all code using Shape will need to be reviewed, possibly changed, and probably recompiled.
    - The implementation of` Shape::move()` is an example of implementation inheritance: we have defined `move()` once and for all for all derived classes. The more code there is in such base class member function implementations and the more data is shared by placing it in the base, the more benefits we gain - and the less stable the hierarchy is.
```cpp
class Shape { // BAD, mixed interface and implementation
  public:
    Shape();
    Shape(Point ce = {0, 0}, Color co = none) : cent{ce}, col{co} { /* ... */
    }

    Point center() const { return cent; }
    Color color() const { return col; }

    virtual void rotate(int) = 0;
    virtual void move(Point p) {
        cent = p;
        redraw();
    }

    virtual void redraw();

    // ...
  private:
    Point cent;
    Color col;
};

class Circle : public Shape {
  public:
    Circle(Point c, int r) : Shape{c}, rad{r} { /* ... */
    }

    // ...
  private:
    int rad;
};

class Triangle : public Shape {
  public:
    Triangle(Point p1, Point p2, Point p3); // calculate center
    // ...
};
```
- This Shape hierarchy can be rewritten using interface inheritance: The interface is now less brittle, but there is more work in implementing the member functions. For example, center has to be implemented by every class derived from Shape.

```cpp
class Shape { // pure interface
  // Note that a pure interface rarely has constructors: there is nothing to construct.
  public:
    virtual Point center() const = 0;
    virtual Color color() const = 0;

    virtual void rotate(int) = 0;
    virtual void move(Point p) = 0;

    virtual void redraw() = 0;

    // ...
};
class Circle : public Shape {
  public:
    Circle(Point c, int r, Color c) : cent{c}, rad{r}, col{c} { /* ... */
    }

    Point center() const override { return cent; }
    Color color() const override { return col; }

    // ...
  private:
    Point cent;
    int rad;
    Color col;
};
```
- **Dual hierarchies** has the benefit of stable hierarchies from implementation hierarchies and the benefit of implementation reuse from implementation inheritance. One way of implementing is multiple-inheritance variant.
  - This can be useful when the implementation class has members that are not offered in the abstract interface or if direct use of a member offers optimization opportunities (e.g., if an implementation member function is `final`).
  - Another (related) technique for separating interface and implementation is [Pimpl](I.md#i27-for-stable-library-abi-consider-the-pimpl-idiom).
- To make this interface useful, we must provide its implementation classes (here, named equivalently, but in the Impl namespace).
```cpp

namespace Interface {
class Shape { // pure interface
  public:
    virtual Point center() const = 0;
    virtual Color color() const = 0;

    virtual void rotate(int) = 0;
    virtual void move(Point p) = 0;

    virtual void redraw() = 0;

    // ...
};

class Circle : public virtual Shape { // pure interface
  public:
    virtual int radius() = 0;
    // ...
};

} // namespace Interface
namespace Impl {
class Shape : public virtual Interface::Shape { // implementation
  public:
    // constructors, destructor
    // ...
    Point center() const override { /* ... */
    }
    Color color() const override { /* ... */
    }

    void rotate(int) override { /* ... */
    }
    void move(Point p) override { /* ... */
    }

    void redraw() override { /* ... */
    }

    // ...
};

class Circle : public virtual Interface::Circle,
               public Impl::Shape { // implementation
  public:
    // constructors, destructor

    int radius() override { /* ... */
    }
    // ...
};

} // namespace Impl

```
- And we could extend the hierarchies by adding a Smiley class (:-)):
```cpp
namespace Interface {
class Smiley : public virtual Circle { // pure interface
  public:
    // ...
};
} // namespace Interface

namespace Impl {
class Smiley : public virtual Interface::Smiley,
               public Impl::Circle { // implementation
  public:
    // constructors, destructor
    // ...
}
} // namespace Impl
```
- There are now two hierarchies:
  - interface: Smiley -> Circle -> Shape
  - implementation: Impl::Smiley -> Impl::Circle -> Impl::Shape
- Since each implementation is derived from its interface as well as its implementation base class we get a lattice (DAG).
```
Smiley     ->         Circle     ->  Shape
  ^                     ^               ^
  |                     |               |
Impl::Smiley -> Impl::Circle -> Impl::Shape
```
- As mentioned, this is just one way to construct a dual hierarchy.
- The implementation hierarchy can be used directly, rather than through the abstract interface.
```cpp
void work_with_shape(Interface::Shape&);

int user() {
    Impl::Smiley my_smiley{/* args */}; // create concrete shape
    // ...
    my_smiley.some_member(); // use implementation class directly
    // ...
    work_with_shape(my_smiley); // use implementation through abstract interface
    // ...
}
```
- Note: There is often a choice between offering common functionality as (implemented) base class functions and free-standing functions (in an implementation namespace).
  - Base classes gives a shorter notation and easier access to shared data (in the base) at the cost of the functionality being available only to users of the hierarchy.

## C.130: For making deep copies of polymorphic classes prefer a virtual clone function instead of public copy construction/assignment
- Copying a polymorphic class is discouraged due to the slicing problem, see [C.67](C.copy.md#c67-a-polymorphic-class-should-suppress-public-copymove).
- If you really need copy semantics, copy deeply: Provide a `virtual clone` function that will copy the actual most-derived type and return an owning pointer to the new object, and then in derived classes return the derived type (use a covariant return type).
```cpp
class B {
  public:
    B() = default;
    virtual ~B() = default;
    virtual gsl::owner<B*> clone() const = 0;

  protected: // C.67
    B(const B&) = default;
    B& operator=(const B&) = default;
    B(B&&) = default;
    B& operator=(B&&) = default;
    // ...
};

class D : public B {
  public:
    gsl::owner<D*> clone() const override { return new D{*this}; };
};
```
- Generally, it is recommended to use smart pointers to represent ownership (see [R.20](R.md#r20-use-uniqueptr-or-sharedptr-to-represent-ownership)).
- However, because of language rules, the covariant return type cannot be a smart pointer: `D::clone` can't return a `unique_ptr<D>` while `B::clone` returns `unique_ptr<B>`.
- Therefore, you either need to consistently return `unique_ptr<B>` in all overrides, or use `owner<>` utility from the Guidelines Support Library.

## C.131: Avoid trivial getters and setters
- A trivial getter or setter adds no semantic value; the data item could just as well be `public`.
- Consider making such a class a `struct`
- The key to this rule is whether the semantics of the getter/setter are trivial. While it is not a complete definition of "trivial", consider whether there would be any difference beyond syntax if the getter/setter was a public data member instead.
- Examples of non-trivial semantics would be: maintaining a class invariant or converting between an internal type and an interface type.

## C.132: Don't make a function virtual without reason
- Redundant `virtual` increases run-time and object-code size. A `virtual` function can be overridden and is thus open to mistakes in a derived class. A `virtual` function ensures code replication in a templated hierarchy.

## C.133: Avoid protected data
- `protected` data is a source of complexity and errors. protected data complicates the statement of invariants.
- `protected` data inherently violates the guidance against putting data in base classes, which usually leads to having to deal with virtual inheritance as well.
- It is up to every derived class to manipulate the protected data correctly. This has been popular, but also a major source of maintenance problems.
  - In a large class hierarchy, the consistent use of protected data is hard to maintain because there can be a lot of code, spread over a lot of classes.
  - The set of classes that can touch that data is open: anyone can derive a new class and start manipulating the protected data.
  - Often, it is not possible to examine the complete set of classes, so any change to the representation of the class becomes infeasible.
  - There is no enforced invariant for the protected data; it is much like a set of global variables. The protected data has de facto become global to a large body of code.
  - Protected data often looks tempting to enable arbitrary improvements through derivation. Often, what you get is unprincipled changes and errors. Prefer private data with a well-specified and enforced invariant. Alternative, and often better, [keep data out of any class used as an interface](#c121-if-a-base-class-is-used-as-an-interface-make-it-a-pure-abstract-class).

## C.134: Ensure all non-const data members have the same access level
- Prevention of logical confusion leading to errors. If the non-const data members don't have the same access level, the type is confused about what it's trying to do. Is it a type that maintains an invariant or simply a collection of values?
- The core question is: What code is responsible for maintaining a meaningful/correct value for that variable? There are exactly two kinds of data members:
  - A: Ones that **don't participate in the object's invariant**. Any combination of values for these members is valid.
  - B: Ones that do participate in the object's invariant.
- Not every combination of values is meaningful (else there'd be no invariant). Therefore all code that has write access to these variables must know about the invariant, know the semantics, and know (and actively implement and enforce) the rules for keeping the values correct.
- Data members in category A should just be `public` (or, more rarely, prote`cted if you only want derived classes to see them). They don't need encapsulation. All code in the system might as well see and manipulate them.

- Data members in category B should be **private** or **const**.
- This is because encapsulation is important. To make them non-private and non-const would mean that the **object can't control its own state: An unbounded amount of code beyond the class would need to know about the invariant and participate in maintaining it accurately** --
  - if these data members were `public`, that would be all calling code that uses the object;
  - if they were `protected`, it would be all the code in current and future derived classes.
- This leads to **brittle and tightly coupled code tha**t quickly becomes a nightmare to maintain. Any code that inadvertently sets the data members to an invalid or unexpected combination of values would corrupt the object and all subsequent uses of the object.
- Most classes are either all A or all B:
  - All public: If you're writing an aggregate bundle-of-variables without an invariant across those variables, then all the variables should be public. By convention, declare such classes `struct` rather than `class`
  - All private: If you're writing a type that maintains an invariant, then all the non-const variables should be `private` -- it should be encapsulated.
- Exception: Occasionally classes will mix A and B, usually for debug reasons.
  - An encapsulated object might contain something like non-const debug instrumentation that isn't part of the invariant and so falls into category A -- it isn't really part of the object's value or meaningful observable state either.
  - In that case, the A parts should be treated as A's (made `public`, or in rarer cases `protected` if they should be visible only to derived classes) and the B parts should still be treated like B's (`private` or `const`).

## C.135: Use multiple inheritance to represent multiple distinct interfaces
- Not all classes will necessarily support all interfaces, and not all callers will necessarily want to deal with all operations. Especially to break apart monolithic interfaces into "aspects" of behavior supported by a given derived class.
- This is a very common use of inheritance because the need for multiple different interfaces to an implementation is common and such interfaces are often not easily or naturally organized into a single-rooted hierarchy.
- Example: `istream` provides the interface to input operations; `ostream` provides the interface to output operations. `iostream` provides the union of the `istream` and `ostream` interfaces and the synchronization needed to allow both on a single stream.
```cpp
class iostream : public istream, public ostream {   // very simplified
    // ...
};
```
## C.136: Use multiple inheritance to represent the union of implementation attributes
- Some forms of mixins have state and often operations on that state. If the operations are `virtual` the use of inheritance is necessary, if not using inheritance can avoid boilerplate and forwarding.
- Example: `istream` provides the interface to input operations (and some data); `ostream` provides the interface to output operations (and some data). `iostream` provides the union of the `istream` and `ostream` interfaces and the synchronization needed to allow both on a single stream.
- This a relatively rare use because implementation can often be organized into a single-rooted hierarchy.
- Sometimes, an "implementation attribute" is more like a "mixin" that determine the behavior of an implementation and inject members to enable the implementation of the policies it requires. For example, see `std::enable_shared_from_this` or various bases from `boost.intrusive` (e.g. `list_base_hook` or `intrusive_ref_counter`).

## C.137: Use virtual bases to avoid overly general base classes
- Allow separation of shared data and interface. To avoid all shared data to being put into an ultimate base class.
- For example:
  - Factoring out `Utility` makes sense if many derived classes share significant "implementation details."
  - `Interface` is the root of an interface hierarchy and `Utility` is the root of an implementation hierarchy.
  - Often, linearization of a hierarchy is a better solution.
```cpp
struct Interface {
    virtual void f();
    virtual int g();
    // ... no data here ...
};

class Utility {  // with data
    void utility1();
    virtual void utility2();    // customization point
public:
    int x;
    int y;
};

class Derive1 : public Interface, virtual protected Utility {
    // override Interface functions
    // Maybe override Utility virtual functions
    // ...
};

class Derive2 : public Interface, virtual protected Utility {
    // override Interface functions
    // Maybe override Utility virtual functions
    // ...
};
```

## C.138: Create an overload set for a derived class and its bases with `using`
- Without a using declaration, member functions in the derived class hide the entire inherited overload sets.
- This issue **affects both virtual and non-virtual member functions**
```cpp
// Example bad
#include <iostream>
class B {
  public:
    virtual int f(int i) {
        std::cout << "f(int): ";
        return i;
    }
    virtual double f(double d) {
        std::cout << "f(double): ";
        return d;
    }
    virtual ~B() = default;
};
class D : public B {
  public:
    int f(int i) override {
        std::cout << "f(int): ";
        return i + 1;
    }
};
int main() {
    D d;
    std::cout << d.f(2) << '\n';   // prints "f(int): 3"
    std::cout << d.f(2.3) << '\n'; // prints "f(int): 3" // shadow base
}
```
```cpp
class D : public B {
  public:
    int f(int i) override {
        std::cout << "f(int): ";
        return i + 1;
    }
    using B::f; // exposes f(double) through using
};
```
- For variadic bases, C++17 introduced a variadic form of the using-declaration:
```cpp
template <class... Ts> struct Overloader : Ts... {
    using Ts::operator()...; // exposes operator() from every base
};
```

## C.139: Use `final` on classes sparingly
- Capping a hierarchy with `final` classes is rarely needed for logical reasons and can be damaging to the extensibility of a hierarchy.
- Not every class is meant to be a base class. Most standard-library classes are examples of that (e.g., `std::vector` and `std::string` are not designed to be derived from). This rule is about using `final` on classes with virtual functions meant to be interfaces for a class hierarchy.
```cpp
class Widget { /* ... */
};

// nobody will ever want to improve My_widget (or so you thought)
class My_widget final : public Widget { /* ... */
};

class My_improved_widget : public My_widget { /* ... */
};                                            // error: can't do that
```
- Capping an **individual `virtual` function with `final` is error-prone as `final` can easily be overlooked when defining/overriding a set of functions.**
  - Fortunately, the compiler catches such mistakes: You cannot re-declare/re-open a final member in a derived class.
- Claims of performance improvements from `final` should be substantiated. **Too often, such claims are based on conjecture or experience with other languages.**
- There are examples where `final` can be important for both logical and performance reasons. One example is a performance-critical AST hierarchy in a **compiler** or **language analysis tool**. New derived classes are not added every year and only by library implementers. However, misuses are (or at least have been) far more common.

## C.140: Do not provide different default arguments for a virtual function and an overrider
- That can cause confusion: **An overrider does not inherit default arguments.**
```cpp
class Base {
public:
    virtual int multiply(int value, int factor = 2) = 0;
    virtual ~Base() = default;
};

class Derived : public Base {
public:
    int multiply(int value, int factor = 10) override;
};

Derived d;
Base& b = d;

b.multiply(10);  // these two calls will call the same function but
d.multiply(10);  // with different arguments and so different results
```