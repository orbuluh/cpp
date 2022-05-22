# C: Classes and class hierarchies
- [C: Classes and class hierarchies](#c-classes-and-class-hierarchies)
  - [C.1: Organize related data into structures (structs or classes)](#c1-organize-related-data-into-structures-structs-or-classes)
  - [C.2: Use class if the class has an invariant; use struct if the data members can vary independently](#c2-use-class-if-the-class-has-an-invariant-use-struct-if-the-data-members-can-vary-independently)
  - [C.3: Represent the distinction between an interface and an implementation using a class](#c3-represent-the-distinction-between-an-interface-and-an-implementation-using-a-class)
  - [C.4: Make a function a member only if it needs direct access to the representation of a class](#c4-make-a-function-a-member-only-if-it-needs-direct-access-to-the-representation-of-a-class)
  - [C.5: Place helper functions in the same namespace as the class they support](#c5-place-helper-functions-in-the-same-namespace-as-the-class-they-support)
  - [C.7: Don't define a class or enum and declare a variable of its type in the same statement](#c7-dont-define-a-class-or-enum-and-declare-a-variable-of-its-type-in-the-same-statement)
  - [C.8: Use `class` rather than `struct` if any member is non-public](#c8-use-class-rather-than-struct-if-any-member-is-non-public)
  - [C.9: Minimize exposure of members](#c9-minimize-exposure-of-members)
- [Subsections](#subsections)


## C.1: Organize related data into structures (structs or classes)

## C.2: Use class if the class has an invariant; use struct if the data members can vary independently
- The use of class alerts the programmer to the need for an invariant. This is a useful convention.
- [Conventional class member declaration order](NL.md#nl16-conventional-class-member-declaration-order)

## C.3: Represent the distinction between an interface and an implementation using a class
- Ideally, and typically, an interface is far more stable than its implementation(s).
- An explicit distinction between interface and implementation improves readability and simplifies maintenance.
- C.3 is one of the way to explicitly distinguish between an interface and its implementation "details, other ways include:
  - use a set of declarations of freestanding functions in a namespace
  - an abstract base class
  - a function template with concepts to represent an interface.
  - ...

## C.4: Make a function a member only if it needs direct access to the representation of a class
- Less coupling than with member functions, fewer functions that can cause trouble by modifying object state, reduces the number of functions that needs to be modified after a change in representation.
```cpp
class Date {
    // ... relatively small interface ...
};

// helper functions have no need for direct access to the representation of a Date
Date next_weekday(Date);
bool operator==(Date, Date);
```
- Exception: The language requires operators =, (), [], and -> to be members.

## C.5: Place helper functions in the same namespace as the class they support
- A helper function is a function (usually supplied by the writer of a class) that does not need direct access to the representation of the class, yet is seen as part of the useful interface to the class.
- Placing them in the same namespace as the class makes their relationship to the class obvious and allows them to be found by argument dependent lookup.
- This is especially important for [overloaded operators](C.over.md#c168-define-overloaded-operators-in-the-namespace-of-their-operands).

## C.7: Don't define a class or enum and declare a variable of its type in the same statement
```cpp
// Bad
struct Data { /* class definitions */ } data{ /* ctor param */ };

// Prefer
struct Data { /* class definitions */ };
Data data{ /* ctor param */ };
```
- Mixing a type definition and the definition of another entity in the same declaration is confusing and unnecessary.

## C.8: Use `class` rather than `struct` if any member is non-public
- Readability. To make it clear that something is being hidden/abstracted. This is a useful convention.
- The private data is hidden far from the public data. The data is split in different parts of the class declaration. Different parts of the data have different access.
- Making `struct` with private interface decreases readability and complicates maintenance.

## C.9: Minimize exposure of members
- Encapsulation. Information hiding. Minimize the chance of unintended access. This simplifies maintenance.
- **If we want to enforce a relation among members, we need to make them `private` and enforce that relation (invariant) through constructors and member functions.**
- If the set of direct users of a set of variables cannot be easily determined, the type or usage of that set cannot be (easily) changed/improved.

# Subsections
- [C.concrete: Concrete types](C.concrete.md)
- [C.defop:    Default Operations](C.defop.md)
- [C.dtor:     Destructors](C.dtor.md)
- [C.ctor:     Constructors, assignments, and destructors](C.ctor.md)
- [C.copy:     Copy and move](C.copy.md)
- [C.other:    Other default operation rules](C.other.md)
- [C.con:      Containers and other resource handles](C.con.md)
- [C.lambdas:  Function objects and lambdas](C.lambdas.md)
- [C.hier:     Class hierarchies (OOP)](C.hier.md)
- [C.hierclss: Designing classes in a hierarchy](C.hierclass.md)
- [C.hier-access: Accessing objects in a hierarchy](C.hier.access.md)
- [C.over:     Overloading and overloaded operators](C.over.md)
- [C.union:    Unions](C.union.md)
