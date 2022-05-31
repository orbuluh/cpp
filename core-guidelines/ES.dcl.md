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


## ES.9: Avoid ALL_CAPS names
- Such names are commonly used for macros. Thus, ALL_CAPS name are vulnerable to unintended macro substitution.

```cpp
// somewhere in some header:
#define NE !=

// somewhere else in some other header:
enum Coord { N, NE, NW, S, SE, SW, E, W };

// somewhere third in some poor programmer's .cpp:
switch (direction) {
case N:
    // ...
case NE:
    // ...
    // ...
}
```
- Note - Do not use ALL_CAPS for constants just because constants used to be macros.

## ES.10: Declare one name (only) per declaration
- One declaration per line increases readability and avoids mistakes related to the C/C++ grammar. It also leaves room for a more descriptive end-of-line comment.
```cppp
char *p, c, a[7], *pp[7], **aa[10];   // yuck!
```
- In a long list of declarators it is easy to overlook an uninitialized variable.
```cpp
int a = 10, b = 11, c = 12, d, e = 14, f = 15;
```
- Exception: A function declaration can contain several function argument declarations.
- Exception: A structured binding (C++17) is specifically designed to introduce several variables:
```cpp
auto [iter, inserted] = m.insert_or_assign(k, val);
if (inserted) { /* new entry was inserted */ }
```
```cpp
template<class InputIterator, class Predicate>
bool any_of(InputIterator first, InputIterator last, Predicate pred);
```
- or better using concepts:
```cpp
bool any_of(input_iterator auto first, input_iterator auto last, predicate auto pred);
```
```cpp
double scalbn(double x, int n);   // OK: x * pow(FLT_RADIX, n); FLT_RADIX is usually 2
```
- or:
```cpp
double scalbn(    // better: x * pow(FLT_RADIX, n); FLT_RADIX is usually 2
    double x,     // base value
    int n         // exponent
);
```
or:
```cpp
// better: base * pow(FLT_RADIX, exponent); FLT_RADIX is usually 2
double scalbn(double base, int exponent);
```

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

## ES.12: Do not reuse names in nested scopes
- It is easy to get confused about which variable is used. Can cause maintenance problems.
```cpp
int d = 0;
// ...
if (cond) {
    // ...
    d = 9;
    // ...
} else {
    // ...
    int d = 7;
    // ...
    d = value_to_be_returned;
    // ...
}

return d;
```
- If this is a large if-statement, it is easy to overlook that a new d has been introduced in the inner scope.
- This is a known source of bugs. Sometimes such reuse of a name in an inner scope is called "shadowing".
- Shadowing is primarily a problem when functions are too large and too complex.
- Shadowing of function arguments in the outermost block is disallowed by the language:
```cpp
void f(int x) {
    int x = 4; // error: reuse of function argument name

    if (x) {
        int x = 7; // allowed, but bad
        // ...
    }
}
```
- Reuse of a member name as a local variable can also be a problem:
```cpp
struct S {
    int m;
    void f(int x);
};

void S::f(int x) {
    m = 7; // assign to member
    if (x) {
        int m = 9;
        // ...
        m = 99; // assign to local variable
        // ...
    }
}
```
- Exception: We often reuse function names from a base class in a derived class:
```cpp
struct B {
    void f(int);
};

struct D : B {
    void f(double);
    using B::f;
};
```
- **This is error-prone.** For example, had we forgotten the using declaration, a call d.f(1) would not have found the int version of f.

## ES.20: Always initialize an object
- Avoid used-before-set errors and their associated undefined behavior. Avoid problems with comprehension of complex initialization. Simplify refactoring.
- The **always initialize rule** is deliberately stronger than the **an object must be set before used language rule**. The latter, more relaxed rule, catches the technical bugs, but:
  - It leads to less readable code
  - It encourages people to declare names in greater than necessary scopes
  - It leads to harder to read code
  - It leads to logic bugs by encouraging complex code
  - It hampers refactoring
- The always initialize rule is a style rule aimed to improve maintainability as well as a rule protecting against used-before-set errors.
- Here is an example that is often considered to demonstrate the need for a more relaxed rule for initialization
```cpp
widget i; // "widget" a type that's expensive to initialize, possibly a large POD
widget j;

if (cond) { // bad: i and j are initialized "late"
    i = f1();
    j = f2();
} else {
    i = f3();
    j = f4();
}
```
- This cannot trivially be rewritten to initialize i and j with initializers. Note that for types with a default constructor, attempting to postpone initialization simply leads to a default initialization followed by an assignment.
- A popular reason for such examples is "efficiency", but a compiler that can detect whether we made a used-before-set error can also eliminate any redundant double initialization.
- Assuming that there is a logical connection between i and j, that connection should probably be expressed in code:
```cpp
pair<widget, widget> make_related_widgets(bool x) {
    return (x) ? {f1(), f2()} : {f3(), f4()};
}
auto [i, j] = make_related_widgets(cond); // C++17
```
- If the `make_related_widgets` function is otherwise redundant, we can eliminate it by using a lambda [ES.28](#es28-use-lambdas-for-complex-initialization-especially-of-const-variables)
```cpp
auto [i, j] = [x] { return (x) ? pair{f1(), f2()} : pair{f3(), f4()} }();    // C++17
```

- Using a value representing "uninitialized" is a symptom of a problem and not a solution:

```cpp
widget i = uninit; // bad
widget j = uninit;

// ...
use(i); // possibly used before set
// ...

if (cond) { // bad: i and j are initialized "late"
    i = f1();
    j = f2();
} else {
    i = f3();
    j = f4();
}
```
- Now the compiler cannot even simply detect a used-before-set. Further, we've introduced complexity in the state space for widget: which operations are valid on an uninit widget and which are not?
- Note: Complex initialization has been popular with clever programmers for decades. It has also been a major source of errors and complexity. Many such errors are introduced during maintenance years after the initial implementation. Example:
```cpp
//This rule covers member variables.
class X {
  public:
    X(int i, int ci) : m2{i}, cm2{ci} {}
    // ...

  private:
    int m1 = 7;
    int m2;
    int m3;

    const int cm1 = 7;
    const int cm2;
    const int cm3;
};
```
- The compiler will flag the uninitialized cm3 because it is a const, but it will not catch the lack of initialization of m3.
- Usually, a rare spurious member initialization is worth the absence of errors from lack of initialization and often an optimizer can eliminate a redundant initialization (e.g., an initialization that occurs immediately before an assignment).
- Exception: If you are declaring an object that is just about to be initialized from input, initializing it would cause a double initialization. However, beware that this might leave uninitialized data beyond the input -- and that has been a fertile source of errors and security breaches:
```cpp
constexpr int max = 8 * 1024;
int buf[max];         // OK, but suspicious: uninitialized
f.read(buf, max);
```
- The cost of initializing that array could be significant in some situations. However, such examples do tend to leave uninitialized variables accessible, so they should be treated with suspicion.
```cpp
constexpr int max = 8 * 1024;
int buf[max] = {};   // zero all elements; better in some situations
f.read(buf, max);
```
- Because of the restrictive initialization rules for arrays and std::array, they offer the most compelling examples of the need for this exception.
- When feasible use a library function that is known not to overflow. For example:
```cpp
string s;   // s is default initialized to ""
cin >> s;   // s expands to hold the string
```
- Don't consider simple variables that are targets for input operations exceptions to this rule:
```cpp
int i;   // bad
// ...
cin >> i;
```
- In the not uncommon case where the input target and the input operation get separated (as they should not) the possibility of used-before-set opens up.
```cpp
int i2 = 0;   // better, assuming that zero is an acceptable value for i2
// ...
cin >> i2;
```
- A good optimizer should know about input operations and eliminate the redundant operation.
- Note: Sometimes, a lambda can be used as an initializer to avoid an uninitialized variable:
```cpp
error_code ec;
Value v = [&] {
    auto p = get_value();   // get_value() returns a pair<error_code, Value>
    ec = p.first;
    return p.second;
}();
```
- or maybe:
```cpp
Value v = [] {
    auto p = get_value();   // get_value() returns a pair<error_code, Value>
    if (p.first) throw Bad_value{p.first};
    return p.second;
}();
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