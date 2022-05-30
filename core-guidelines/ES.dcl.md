# ES.dcl: Declarations
- A declaration is a statement. A declaration introduces a name into a scope and might cause the construction of a named object.

## ES.5: Keep scopes small
- Readability. Minimize resource retention. Avoid accidental misuse of value.
- Alternative formulation: Don't declare a name in an unnecessarily large scope.

## ES.6: Declare names in for-statement initializers and conditions to limit scope
- Readability. Limit the loop variable visibility to the scope of the loop. Avoid using the loop variable for other purposes after the loop.
- Minimize resource retention.

```cpp
void use() {
    for (string s; cin >> s;)
        v.push_back(s);

    for (int i = 0; i < 20; ++i) { // good: i is local to for-loop
        // ...
    }

    if (auto pc =
            dynamic_cast<Circle*>(ps)) { // good: pc is local to if-statement
        // ... deal with Circle ...
    } else {
        // ... handle error ...
    }
}
```
- C++17 and C++20 example
```cpp
// Note: C++17 and C++20 also add if, switch, and range-for initializer statements.
// These require C++17 and C++20 support.

std::map<int, string> mymap;
if (auto result = mymap.insert(value); result.second) {
    // insert succeeded, and result is valid for this block
    use(result.first); // ok
    // ...
} // result is destroyed here
```

## ES.7: Keep common and local names short, and keep uncommon and non-local names longer
- Readability. Lowering the chance of clashes between unrelated non-local names.

```cpp
// Conventional short, local names increase readability:
template <typename T> // good
void print(ostream& os, const vector<T>& v) {
    for (gsl::index i = 0; i < v.size(); ++i)
        os << v[i] << '\n';
}
```
- An index is conventionally called i and there is no hint about the meaning of the vector in this generic function, so v is as good name as any. Compare ...
```cpp
template <typename Element_type> // bad: verbose, hard to read
void print(ostream& target_stream, const vector<Element_type>& current_vector) {
    for (gsl::index current_element_index = 0;
         current_element_index < current_vector.size(); ++current_element_index)
        target_stream << current_vector[current_element_index] << '\n';
}
```
- Unconventional and short non-local names obscure code:
```cpp
void use1(const string& s) {
    // ...
    tt(s); // bad: what is tt()?
    // ...
}
// Better, give non-local entities readable names:
void use1(const string& s) {
    // ...
    trim_tail(s); // better
    // ...
}
```
- Here, there is a chance that the reader knows what trim_tail means and that the reader can remember it after looking it up.
- Argument names of large functions are de facto non-local and should be meaningful
  - We recommend keeping functions short, but that rule isn't universally adhered to and naming should reflect that.
```cpp
void complicated_algorithm(vector<Record>& vr, const vector<int>& vi, map<string, int>& out)
// read from events in vr (marking used Records) for the indices in
// vi placing (name, index) pairs into out
{
    // ... 500 lines of code using vr, vi, and out ...
}
//
```

## ES.8: Avoid similar-looking names
- Code clarity and readability. Too-similar names slow down comprehension and increase the likelihood of error.
```cpp
// Example, bad
if (readable(i1 + l1 + ol + o1 + o0 + ol + o1 + I0 + l0)) surprise();

// Example, bad
// Do not declare a non-type with the same name as a type in the same scope.
// This removes the need to disambiguate with a keyword such as struct or enum.
// It also removes a source of errors, as struct X can implicitly declare X if lookup fails.

struct foo { int n; };
struct foo foo();       // BAD, foo is a type already in scope
struct foo x = foo();   // requires disambiguation
```
- Exception: Antique header files might declare non-types and types with the same name in the same scope.







## ES.11: Use `auto` to avoid redundant repetition of type names

- Simple repetition is tedious and error-prone.
- When you use `auto`, the name of the declared entity is in a fixed position in the declaration, increasing readability.
- In a function template declaration the return type can be a member type.
```cpp
auto p = v.begin();      // vector<DataRecord>::iterator
auto z1 = v[3];          // makes copy of DataRecord
auto& z2 = v[3];         // avoids copy
const auto& z3 = v[3];   // const and avoids copy
auto h = t.future();
auto q = make_unique<int[]>(s);
auto f = [](int x) { return x + 10; };
```
- In each case, we save writing a longish, hard-to-remember type that the compiler already knows but a programmer could get wrong.

```cpp
template<class T>
auto Container<T>::first() -> Iterator;   // Container<T>::Iterator
```
- Exception: Avoid `auto` for initializer lists **and in cases where you know exactly which type you want and where an initializer might require conversion.**
```cpp
auto lst = { 1, 2, 3 };   // lst is an initializer list
auto x{1};   // x is an int (in C++17; initializer_list in C++11)
```
- Note, As of C++20, we can (and should) use concepts to be more specific about the type we are deducing:
```cpp
forward_iterator auto p = algo(x, y, z);
//Example (C++17)
std::set<int> values;
// ...
auto [ position, newly_inserted ] = values.insert(5);   // break out the members of the std::pair
```

## ES.28: Use lambdas for complex initialization, especially of const variables
- It nicely encapsulates local initialization, including cleaning up scratch variables needed only for the initialization, without needing to create a needless non-local yet non-reusable function.
- It also works for variables that should be const but only after some initialization work.
- If at all possible, reduce the conditions to a simple set of alternatives (e.g., an enum) and don't mix up selection and initialization.
```cpp
// Bad
widget x;                               // should be const, but:
for (auto i = 2; i <= N; ++i) {         // this could be some
    x += some_obj.do_something_with(i); // arbitrarily long code
} // needed to initialize x
// from here, x should be const, but we can't say so in code in this style
```
```cpp
// Prefer
const widget x = [&] {
    widget val; // assume that widget has a default constructor
    for (auto i = 2; i <= N; ++i) {           // this could be some
        val += some_obj.do_something_with(i); // arbitrarily long code
    }                                         // needed to initialize x
    return val;
}();
```