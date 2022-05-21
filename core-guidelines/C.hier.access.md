# C.hier-access: Accessing objects in a hierarchy

- [C.hier-access: Accessing objects in a hierarchy](#chier-access-accessing-objects-in-a-hierarchy)
  - [C.145: Access polymorphic objects through pointers and references](#c145-access-polymorphic-objects-through-pointers-and-references)
  - [C.146: Use `dynamic_cast` where class hierarchy navigation is unavoidable](#c146-use-dynamic_cast-where-class-hierarchy-navigation-is-unavoidable)
  - [C.147: Use `dynamic_cast` to a **reference** type when failure to find the required class is considered an **error**](#c147-use-dynamic_cast-to-a-reference-type-when-failure-to-find-the-required-class-is-considered-an-error)
  - [C.148: Use `dynamic_cast` to a **pointer** type when failure to find the required class is considered a **valid alternative**](#c148-use-dynamic_cast-to-a-pointer-type-when-failure-to-find-the-required-class-is-considered-a-valid-alternative)
  - [C.149: Use `unique_ptr` or `shared_ptr` to avoid forgetting to delete objects created using new](#c149-use-unique_ptr-or-shared_ptr-to-avoid-forgetting-to-delete-objects-created-using-new)
  - [C.150: Use `make_unique()` to construct objects owned by `unique_ptr`s](#c150-use-make_unique-to-construct-objects-owned-by-unique_ptrs)
  - [C.152: Never assign a pointer to an array of derived class objects to a pointer to its base](#c152-never-assign-a-pointer-to-an-array-of-derived-class-objects-to-a-pointer-to-its-base)
  - [C.153: Prefer virtual function to casting](#c153-prefer-virtual-function-to-casting)

## C.145: Access polymorphic objects through pointers and references
- If you have a class with a virtual function, you don't (in general) know which class provided the function to be used.
- Consider [C.67: A polymorphic class should suppress public copy/move](C.copy.md#c67-a-polymorphic-class-should-suppress-public-copymove)
```cpp
struct B {
    int a;
    virtual int f();
    virtual ~B() = default
};
struct D : B {
    int b;
    int f() override;
};

void use(B b) {
    D d;
    B b2 = d; // slice
    B b3 = b;
}

void use2() {
    D d;
    d.f(); //ok
    use(d); // slice...
}
```

## C.146: Use `dynamic_cast` where class hierarchy navigation is unavoidable
- `dynamic_cast` is checked at run time.
```cpp
struct B {   // an interface
    virtual void f();
    virtual void g();
    virtual ~B();
};

struct D : B {   // a wider interface
    void f() override;
    virtual void h();
};

void user(B* pb)
{
    if (D* pd = dynamic_cast<D*>(pb)) {
        // ... use D's interface ...
    }
    else {
        // ... make do with B's interface ...
    }
}
```
- Use of the other casts can violate type safety and cause the program to access a variable that is actually of type X to be accessed as if it were of an unrelated type Z
```cpp
void user2(B* pb) // bad
{
    D* pd =
        static_cast<D*>(pb); // I know that pb really points to a D; trust me
    // ... use D's interface ...
}

void user3(B* pb) // unsafe
{
    if (some_condition) {
        D* pd = static_cast<D*>(pb); // I know that pb really points to a D; trust me
        // ... use D's interface ...
    } else {
        // ... make do with B's interface ...
    }
}

void f() {
    B b;
    user(&b);  // OK
    user2(&b); // bad error
    user3(&b); // OK *if* the programmer got the some_condition check right
}
```
- Like other casts, `dynamic_cast` is overused.
- **Prefer virtual functions to casting.**
- **Prefer static polymorphism to hierarchy navigation where it is possible (no run-time resolution necessary) and reasonably convenient.**
- Some people use `dynamic_cast` where a `typeid` would have been more appropriate;
  - `dynamic_cast` is a general "is kind of" operation for discovering the best interface to an object, whereas `typeid` is a "give me the exact type of this object" operation to discover the actual type of an object.
  - `typeid` is an inherently simpler operation that ought to be faster.
  - `typeid` is easily hand-crafted if necessary (e.g., if working on a system where RTTI (run-time type information or run-time type identification (RTTI)) is -- for some reason -- prohibited), the former (`dynamic_cast`) is far harder to implement correctly in general.
  - Consider ...
```cpp
struct B {
    const char* name{"B"};
    // if pb1->id() == pb2->id() *pb1 is the same type as *pb2
    virtual const char* id() const { return name; }
    // ...
};

struct D : B {
    const char* name{"D"};
    const char* id() const override { return name; }
    // ...
};

void use() {
    B* pb1 = new B;
    B* pb2 = new D;

    cout << pb1->id(); // "B"
    cout << pb2->id(); // ideally return "D" ... but it is actually implementation defined... (WTF?)

    if (pb1->id() == "D") { // looks innocent
        D* pd = static_cast<D*>(pb1);
        // ...
    }
    // ...
}
```
- Exception: If your implementation provided a really slow dynamic_cast, you might have to use a workaround.
  - However, all workarounds that cannot be statically resolved involve explicit casting (typically `static_cast`) and are error-prone.
  - You will basically be crafting your own special-purpose `dynamic_cast`.
  - So, **first make sure that your `dynamic_cast` really is as slow as you think it is** (there are a fair number of unsupported rumors about) and that your use of `dynamic_cast` is really performance critical.
  - We are of the opinion that current implementations of `dynamic_cast` are **unnecessarily slow**. For example, under suitable conditions, it is possible to perform a `dynamic_cast` in fast constant time. However, **compatibility makes changes difficult even if all agree that an effort to optimize is worthwhile.**
  - In very rare cases, if you have measured that the `dynamic_cast` overhead is material, you have other means to statically guarantee that a downcast will succeed (e.g., you are using CRTP carefully), and there is no virtual inheritance involved, consider tactically resorting `static_cast` **with a prominent comment and disclaimer summarizing this paragraph and that human attention is needed under maintenance because the type system can't verify correctness.**
  - Even so, in our experience such "I know what I'm doing" situations are still a known bug source.
  - Consider:
```cpp
// CRTP
template<typename B>
class Dx : B {
    // ...
};
```
- More about type safety - check [Pro.type](Pro.md#pro-profiles)

## C.147: Use `dynamic_cast` to a **reference** type when failure to find the required class is considered an **error**
- Casting to a reference expresses that you intend to end up with a valid object, so the cast must succeed. `dynamic_cast` will then throw if it does not succeed.
```cpp
std::string f(Base& b) {
    // critical - we thing b must be cast to Derived and agree if it's not, this should throw.
    return dynamic_cast<Derived&>(b).to_string();
}
```

## C.148: Use `dynamic_cast` to a **pointer** type when failure to find the required class is considered a **valid alternative**
- The `dynamic_cast` conversion allows to test whether a pointer is pointing at a polymorphic object that has a given class in its hierarchy. Since failure to find the class merely returns a null value, it can be tested during run time.
- This allows writing code that can choose alternative paths depending on the results.
- Contrast with C.147, where failure is an error, and should not be used for conditional execution.
- Example:
The example below describes the `add` function of a `Shape_owner` that takes ownership of constructed Shape objects. The objects are also sorted into `views`, according to their geometric attributes. In this example, `Shape` does not inherit from `Geometric_attributes`. Only its subclasses do.
```cpp
void add(Shape* const item) {
    // Ownership is always taken
    owned_shapes.emplace_back(item);

    // Check the Geometric_attributes and add the shape to none/one/some/all of
    // the views

    if (auto even = dynamic_cast<Even_sided*>(item)) {
        view_of_evens.emplace_back(even);
    }

    if (auto trisym = dynamic_cast<Trilaterally_symmetrical*>(item)) {
        view_of_trisyms.emplace_back(trisym);
    }
}
```
- A failure to find the required class will cause `dynamic_cast` to return a null value, and de-referencing a null-valued pointer will lead to undefined behavior. Therefore the result of **the `dynamic_cast` should always be treated as if it might contain a null value, and tested.**

## C.149: Use `unique_ptr` or `shared_ptr` to avoid forgetting to delete objects created using new
- [R.23](R.md#r23-use-makeunique-to-make-uniqueptrs)

## C.150: Use `make_unique()` to construct objects owned by `unique_ptr`s
- [R.23](R.md#r22-use-makeshared-to-make-sharedptrs)

## C.152: Never assign a pointer to an array of derived class objects to a pointer to its base
- **Subscripting the resulting base pointer will lead to invalid object access and probably to memory corruption.**
```cpp
struct B {
    int x;
};
struct D : B {
    int y;
};

void use(B*);

D arrDs[] = {{1, 2}, {3, 4}, {5, 6}};
B* p = arrDs;   // bad: a decays to &arrDs[0] which is converted to a B*
p[1].x = 7; // overwrite arrDs[0].y

use(arrDs); // bad: arrDs decays to &arrDs[0] which is converted to a B*
```

## C.153: Prefer virtual function to casting
- A virtual function call is safe, whereas casting is error-prone.
- A virtual function call **reaches the most derived function**, whereas a c**ast might reach an intermediate class and therefore give a wrong result** (especially as a hierarchy is modified during maintenance).