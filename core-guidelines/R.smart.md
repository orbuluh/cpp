# R.smart: Smart pointers

## R.20: Use `unique_ptr` or `shared_ptr` to represent ownership
- Consider:
```cpp
void f()
{
    X x;
    X* p1 { new X };              // will leak!
    std::unique_ptr<X> p2 { new X };   // unique ownership; see also ???
    std::shared_ptr<X> p3 { new X };   // shared ownership; see also ???
    auto p4 = std::make_unique<X>();   // unique_ownership, preferable to the explicit use "new"
    auto p5 = std::make_shared<X>();   // shared ownership, preferable to the explicit use "new"
}
```

## R.21: Prefer `unique_ptr` over `shared_ptr` unless you need to share ownership
- A `unique_ptr` is conceptually simpler and more predictable (you know when destruction happens) and faster (you don't implicitly maintain a use count).

## R.22: Use `make_shared()` to make `shared_ptr`s
- `make_shared` gives a more concise statement of the construction. It also gives an opportunity to** eliminate a separate allocation for the reference counts**, by placing the shared_ptr's use counts next to its object.
```cpp
shared_ptr<X> p1{new X{2}}; // bad
auto p = make_shared<X>(2); // good
```

## R.23: Use `make_unique()` to make `unique_ptr`s
- `make_unique` gives a more concise statement of the construction. It also ensures exception safety in complex expressions.

## R.24: Use `std::weak_ptr` to break cycles of `std::shared_ptrs`
- `shared_ptr`'s rely on use counting and the **use count for a cyclic structure never goes to zer**o, so we need a mechanism to be able to destroy a cyclic structure.
  - As breaking cycles is what you must do; temporarily sharing ownership (through `weak_pointer`) is how you do it.
  - (e.g. You could "temporarily share ownership" simply by using another `shared_ptr` - but it won't break the cycles)

```cpp
#include <memory>

class bar;

class foo {
  public:
    explicit foo(const std::shared_ptr<bar>& forward_reference)
        : forward_reference_(forward_reference) {}

  private:
    std::shared_ptr<bar> forward_reference_;
};

class bar {
  public:
    explicit bar(const std::weak_ptr<foo>& back_reference)
        : back_reference_(back_reference) {}
    void do_something() {
        if (auto shared_back_reference = back_reference_.lock()) {
            // Use *shared_back_reference
        }
    }

  private:
    std::weak_ptr<foo> back_reference_;
};
```

## R.30: Take smart pointers as parameters only to explicitly express lifetime semantics
- [F.7](F.md)
