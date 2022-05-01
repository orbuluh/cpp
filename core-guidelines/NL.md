# NL: Naming and layout rules
## NL.16: Conventional class member declaration order
- Prefer to place the interface first in a class
- When declaring a class use the following order
  - types: classes, enums, and aliases (using)
  - constructors, assignments, destructor
  - functions
  - data
- Use the public before protected before private order.
```cpp
class X {
public:
    // interface
protected:
    // unchecked function for use by derived class implementations
private:
    // implementation details
};
```
- Avoid multiple blocks of declarations of one access.
```cpp
class X {   // bad
public:
    void f();
public:
    int g();
    // ...
};
```