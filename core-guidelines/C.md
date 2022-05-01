# C: Classes and class hierarchies
## C.1 Organize related data into structures (structs or classes)
## C.2 Use class if the class has an invariant; use struct if the data members can vary independently
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

## TBR: C.4