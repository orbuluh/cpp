# C.hier: Class hierarchies (OOP)
- A class hierarchy is constructed to represent a set of hierarchically organized concepts (only).
- Typically base classes act as interfaces. There are two major uses for hierarchies, often named **implementation inheritance** and **interface inheritance**

## C.120: Use class hierarchies to represent concepts with inherent hierarchical structure (only)
- Direct representation of ideas in code eases comprehension and maintenance.
- Make sure the **idea represented in the base class exactly matches all derived types and there is not a better way to express it than using the tight coupling of inheritance.**
- Do not use inheritance when simply having a data member will do.
  - Usually this means that the derived type needs to override a base virtual function or needs access to a protected member.
```cpp
// Example bad: Do not represent non-hierarchical domain concepts as class hierarchies.
template <typename T> class Container {
  public:
    // list operations:
    virtual T& get() = 0;
    virtual void put(T&) = 0;
    virtual void insert(Position) = 0;
    // ...
    // vector operations:
    virtual T& operator[](int) = 0;
    virtual void sort() = 0;
    // ...
    // tree operations:
    virtual void balance() = 0;
    // ...
};
```
- Here most overriding classes cannot implement most of the functions required in the interface well. Thus the base class becomes an implementation burden.
- Furthermore, the user of Container cannot rely on the member functions actually performing meaningful operations reasonably efficiently; it might throw an exception instead.
- Thus users have to resort to run-time checking and/or not using this (over)general interface in favor of a particular interface found by a run-time type inquiry (e.g., a `dynamic_cast`).

```cpp
// Example that using inheritance makes sense
class DrawableUIElement {
  public:
    virtual void render() const = 0;
    // ...
};

class AbstractButton : public DrawableUIElement {
  public:
    virtual void onClick() = 0;
    // ...
};

class PushButton : public AbstractButton {
    void render() const override;
    void onClick() override;
    // ...
};

class Checkbox : public AbstractButton {
    // ...
};
```

## C.121: If a base class is used as an interface, make it a pure abstract class
- A class is more stable if it does not contain data. Interfaces should normally be composed **entirely of public pure virtual functions and a default/empty virtual destructor.**

## C.122: Use abstract classes as interfaces when complete separation of interface and implementation is needed
- Such as on an ABI (link) boundary.
  - A user can now use `D1`s and `D2`s interchangeably through the interface provided by `Device`. 
  - Furthermore, we can update `D1` and `D2` in ways that are not binary compatible with older versions as long as all access goes through `Device`.
```cpp
struct Device {
    virtual ~Device() = default;
    virtual void write(span<const char> outbuf) = 0;
    virtual void read(span<char> inbuf) = 0;
};

class D1 : public Device {
    // ... data ...

    void write(span<const char> outbuf) override;
    void read(span<char> inbuf) override;
};

class D2 : public Device {
    // ... different data ...

    void write(span<const char> outbuf) override;
    void read(span<char> inbuf) override;
};
```
