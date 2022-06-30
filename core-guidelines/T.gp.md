# T.gp: Generic programming

- [T.gp: Generic programming](#tgp-generic-programming)
  - [T.1: Use templates to raise the level of abstraction of code](#t1-use-templates-to-raise-the-level-of-abstraction-of-code)
  - [T.2: Use templates to express algorithms that apply to many argument types](#t2-use-templates-to-express-algorithms-that-apply-to-many-argument-types)
  - [T.3: Use templates to express containers and ranges](#t3-use-templates-to-express-containers-and-ranges)
  - [T.4: Use templates to express syntax tree manipulation](#t4-use-templates-to-express-syntax-tree-manipulation)
  - [T.5: Combine generic and OO techniques to amplify their strengths, not their costs](#t5-combine-generic-and-oo-techniques-to-amplify-their-strengths-not-their-costs)


## T.1: Use templates to raise the level of abstraction of code
- Generality. Reuse. Efficiency.
- Encourages consistent definition of user types.

Example, bad
- Conceptually, the following requirements are wrong because what we want of `T` is more than just the very low-level concepts of "can be incremented" or "can be added":
```cpp
template <typename T>
requires Incrementable<T> T sum1(vector<T>& v, T s) {
    for (auto x : v)
        s += x;
    return s;
}

template <typename T>
requires Simple_number<T> T sum2(vector<T>& v, T s) {
    for (auto x : v)
        s = s + x;
    return s;
}
```
- Assuming that Incrementable does not support + and Simple_number does not support +=, we have **overconstrained** implementers of `sum1` and `sum2`.
- And, in this case, missed an opportunity for a generalization.

```cpp
template <typename T>
requires Arithmetic<T> T sum(vector<T>& v, T s) {
    for (auto x : v)
        s += x;
    return s;
}
```
- Assuming that Arithmetic requires both + and +=, we have constrained the user of sum to provide a complete arithmetic type.
- That is not a minimal requirement, but it gives the implementer of algorithms much needed freedom and ensures that any `Arithmetic` type can be used for a wide variety of algorithms.
- For additional generality and reusability, we could also use a more general `Container` or `Range` concept instead of committing to only one container, `vector`.
- Note: If we define a template to require exactly the operations required for a single implementation of a single algorithm (e.g., requiring just `+=` rather than also `=` and `+`) and only those, we have overconstrained maintainers.
- We aim to **minimize requirements on template arguments, but the absolutely minimal requirements of an implementation is rarely a meaningful concept**.
- Note: Templates can be used to express essentially everything (they are Turing complete), but the aim of generic programming (as expressed using templates) is to **efficiently generalize operations/algorithms over a set of types with similar semantic properties.**

## T.2: Use templates to express algorithms that apply to many argument types
- Generality. Minimizing the amount of source code. Interoperability. Reuse.
- Example: That's the foundation of the STL. A single find algorithm easily works with any kind of input range:
```cpp
template <typename Iter, typename Val>
// requires Input_iterator<Iter>
//       && Equality_comparable<Value_type<Iter>, Val>
Iter find(Iter b, Iter e, Val v) {
    // ...
}
```
- Note: Don't use a template **unless you have a realistic need for more than one template argument type**. Don't overabstract.

## T.3: Use templates to express containers and ranges
- Containers need an element type, and expressing that as a template argument is general, reusable, and type safe.
- It also avoids brittle or inefficient workarounds. Convention: That's the way the STL does it.

```cpp

template <typename T>
// requires Regular<T>
class Vector {
    // ...
    T* elem; // points to sz Ts
    int sz;
};

Vector<double> v(10);
v[7] = 9.9;

// Example, bad
class Container {
    // ...
    void* elem; // points to size elements of some type
    int sz;
};
Container c(10, sizeof(double));
((double*)c.elem)[7] = 9.9;
```
- This doesn't directly express the intent of the programmer and hides the structure of the program from the type system and optimizer.
- Hiding the `void*` behind macros simply obscures the problems and introduces new opportunities for confusion.
- Exceptions: If you need an ABI-stable interface, you might have to provide a base implementation and express the (type-safe) template in terms of that.

## T.4: Use templates to express syntax tree manipulation
- no details

## T.5: Combine generic and OO techniques to amplify their strengths, not their costs
- Generic and OO techniques are complementary.
- Example: Static helps dynamic: Use static polymorphism to implement dynamically polymorphic interfaces.
```cpp
class Command {
    // pure virtual functions
};

// implementations
template </*...*/> class ConcreteCommand : public Command {
    // implement virtuals
};
```
- Example: Dynamic helps static: Offer a generic, comfortable, statically bound interface, but internally dispatch dynamically, so you offer a uniform object layout.
  - Examples include **type erasure** as with `std::shared_ptr`'s deleter (but don't overuse type erasure).
```cpp
#include <memory>

class Object {
  public:
    template <typename T>
    Object(T&& obj)
        : concept_(std::make_shared<ConcreteCommand<T>>(std::forward<T>(obj))) {
    }

    int get_id() const { return concept_->get_id(); }

  private:
    struct Command {
        virtual ~Command() {}
        virtual int get_id() const = 0;
    };

    template <typename T> struct ConcreteCommand final : Command {
        ConcreteCommand(T&& obj) noexcept : object_(std::forward<T>(obj)) {}
        int get_id() const final { return object_.get_id(); }

      private:
        T object_;
    };

    std::shared_ptr<Command> concept_;
};

class Bar {
  public:
    int get_id() const { return 1; }
};

struct Foo {
  public:
    int get_id() const { return 2; }
};

Object o(Bar{});
Object o2(Foo{});
```
- Note: In a **class template, non-virtual functions are only instantiated if they're used** -- but **`virtual` functions are instantiated every time.**
- This can bloat code size, and might overconstrain a generic type by instantiating functionality that is never needed.
- Avoid this, even though the standard-library facets made this mistake.

