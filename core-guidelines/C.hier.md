# C.hier: Class hierarchies (OOP)

## C.121: If a base class is used as an interface, make it a pure abstract class
- A class is more stable if it does not contain data. Interfaces should normally be composed **entirely of public pure virtual functions and a default/empty virtual destructor.**


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




## C.133: Avoid protected data
- `protected` data is a source of complexity and errors. protected data complicates the statement of invariants.
- `protected` data inherently violates the guidance against putting data in base classes, which usually leads to having to deal with virtual inheritance as well.
- It is up to every derived class to manipulate the protected data correctly. This has been popular, but also a major source of maintenance problems.
  - In a large class hierarchy, the consistent use of protected data is hard to maintain because there can be a lot of code, spread over a lot of classes.
  - The set of classes that can touch that data is open: anyone can derive a new class and start manipulating the protected data.
  - Often, it is not possible to examine the complete set of classes, so any change to the representation of the class becomes infeasible.
  - There is no enforced invariant for the protected data; it is much like a set of global variables. The protected data has de facto become global to a large body of code.
  - Protected data often looks tempting to enable arbitrary improvements through derivation. Often, what you get is unprincipled changes and errors. Prefer private data with a well-specified and enforced invariant. Alternative, and often better, [keep data out of any class used as an interface](#c121-if-a-base-class-is-used-as-an-interface-make-it-a-pure-abstract-class).

