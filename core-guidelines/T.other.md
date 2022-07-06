# Other template rules

- [Other template rules](#other-template-rules)
  - [T.140: Name all operations with potential for reuse](#t140-name-all-operations-with-potential-for-reuse)
  - [T.141: Use an unnamed lambda if you need a simple function object in one place only](#t141-use-an-unnamed-lambda-if-you-need-a-simple-function-object-in-one-place-only)
  - [T.142: Use template variables to simplify notation](#t142-use-template-variables-to-simplify-notation)
  - [T.143: Don't write unintentionally non-generic code](#t143-dont-write-unintentionally-non-generic-code)
  - [T.144: Don't specialize function templates](#t144-dont-specialize-function-templates)
  - [T.150: Check that a class matches a `concept` using `static_assert`](#t150-check-that-a-class-matches-a-concept-using-static_assert)

## T.140: Name all operations with potential for reuse
- Documentation, readability, opportunity for reuse.

```cpp
struct Rec {
    string name;
    string addr;
    int id; // unique identifier
};

bool same(const Rec& a, const Rec& b) { return a.id == b.id; }

vector<Rec*> find_id(const string& name); // find all records for "name"

auto x = find_if(vr.begin(), vr.end(), [&](Rec& r) {
    if (r.name.size() != n.size())
        return false; // name to compare to is in n
    for (int i = 0; i < r.name.size(); ++i)
        if (tolower(r.name[i]) != tolower(n[i]))
            return false;
    return true;
});
```
- There is a useful function lurking here (case insensitive string comparison), as there often is when lambda arguments get large.
```cpp
bool compare_insensitive(const string& a, const string& b) {
    if (a.size() != b.size())
        return false;
    for (int i = 0; i < a.size(); ++i)
        if (tolower(a[i]) != tolower(b[i]))
            return false;
    return true;
}

auto x = find_if(vr.begin(), vr.end(),
                 [&](Rec& r) { compare_insensitive(r.name, n); });
```
- Or maybe (if you prefer to avoid the implicit name binding to n):
```cpp
auto cmp_to_n = [&n](const string& a) { return compare_insensitive(a, n); };

auto x = find_if(vr.begin(), vr.end(),
    [](const Rec& r) { return cmp_to_n(r.name); }
);
```
- Exception: Lambdas logically used only locally, such as an argument to `for_each` and similar control flow algorithms.
- Lambdas as initializers


## T.141: Use an unnamed lambda if you need a simple function object in one place only
- That makes the code concise and gives better locality than alternatives.

```cpp
auto earlyUsersEnd = std::remove_if(users.begin(), users.end(),
                                    [](const User &a) { return a.id > 100; });
```
- Exception: Naming a lambda can be useful for clarity even if it is used only once.


## T.142: Use template variables to simplify notation
- Improved readability.


## T.143: Don't write unintentionally non-generic code
- Generality. Reusability. Don't gratuitously commit to details; use the most general facilities available.
- Example: Use != instead of < to compare iterators; != works for more objects because it doesn't rely on ordering.
```cpp
for (auto i = first; i < last; ++i) {   // less generic
    // ...
}

for (auto i = first; i != last; ++i) {   // good; more generic
    // ...
}

// Of course, range-for is better still where it does what you want.
```

- Example: Use the least-derived class that has the functionality you need.
```cpp
class Base {
  public:
    Bar f();
    Bar g();
};

class Derived1 : public Base {
  public:
    Bar h();
};

class Derived2 : public Base {
  public:
    Bar j();
};

// bad, unless there is a specific reason for limiting to Derived1 objects only
void my_func(Derived1& param) {
    use(param.f());
    use(param.g());
}

// good, uses only Base interface so only commit to that
void my_func(Base& param) {
    use(param.f());
    use(param.g());
}
```

## T.144: Don't specialize function templates
- **You can't partially specialize a function template per language rules.**
- **You can fully specialize a function template but you almost certainly want to overload instead** -- because **function template specializations don't participate in overloading**, they don't act as you probably wanted.
- Rarely, you should actually specialize by delegating to a class template that you can specialize properly.
- Exceptions: If you do have a valid reason to specialize a function template, just write a single function template that delegates to a class template, then specialize the class template (including the ability to write partial specializations).

## T.150: Check that a class matches a `concept` using `static_assert`
- If you intend for a class to match a concept, verifying that early saves users' pain.

```cpp
Example
class X {
public:
    X() = delete;
    X(const X&) = default;
    X(X&&) = default;
    X& operator=(const X&) = default;
    // ...
};
```
- Somewhere, possibly in an implementation file, let the compiler check the desired properties of `X`:
```cpp
static_assert(Default_constructible<X>);    // error: X has no default constructor
static_assert(Copyable<X>);                 // error: we forgot to define X's move constructor
```