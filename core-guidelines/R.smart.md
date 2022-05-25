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



## R.22: Use `make_shared()` to make `shared_ptr`s
- `make_shared` gives a more concise statement of the construction. It also gives an opportunity to** eliminate a separate allocation for the reference counts**, by placing the shared_ptr's use counts next to its object.
```cpp
shared_ptr<X> p1{new X{2}}; // bad
auto p = make_shared<X>(2); // good
```

## R.23: Use `make_unique()` to make `unique_ptr`s
- `make_unique` gives a more concise statement of the construction. It also ensures exception safety in complex expressions.