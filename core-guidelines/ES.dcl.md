# ES.dcl: Declarations
- A declaration is a statement. A declaration introduces a name into a scope and might cause the construction of a named object.

- [ES.dcl: Declarations](#esdcl-declarations)
  - [ES.5: Keep scopes small](#es5-keep-scopes-small)
  - [ES.6: Declare names in for-statement initializers and conditions to limit scope](#es6-declare-names-in-for-statement-initializers-and-conditions-to-limit-scope)
  - [ES.7: Keep common and local names short, and keep uncommon and non-local names longer](#es7-keep-common-and-local-names-short-and-keep-uncommon-and-non-local-names-longer)
  - [ES.8: Avoid similar-looking names](#es8-avoid-similar-looking-names)
  - [ES.9: Avoid ALL_CAPS names](#es9-avoid-all_caps-names)
  - [ES.10: Declare one name (only) per declaration](#es10-declare-one-name-only-per-declaration)
  - [ES.11: Use `auto` to avoid redundant repetition of type names](#es11-use-auto-to-avoid-redundant-repetition-of-type-names)
  - [ES.12: Do not reuse names in nested scopes](#es12-do-not-reuse-names-in-nested-scopes)
  - [ES.20: Always initialize an object](#es20-always-initialize-an-object)
  - [ES.21: Don't introduce a variable (or constant) before you need to use it](#es21-dont-introduce-a-variable-or-constant-before-you-need-to-use-it)
  - [ES.22: Don't declare a variable until you have a value to initialize it with](#es22-dont-declare-a-variable-until-you-have-a-value-to-initialize-it-with)
  - [ES.23: Prefer the {}-initializer syntax](#es23-prefer-the--initializer-syntax)
  - [ES.24: Use a `unique_ptr<T>` to hold pointers](#es24-use-a-unique_ptrt-to-hold-pointers)
  - [ES.25: Declare an object `const` or `constexpr` unless you want to modify its value later on](#es25-declare-an-object-const-or-constexpr-unless-you-want-to-modify-its-value-later-on)
  - [ES.26: Don't use a variable for two unrelated purposes](#es26-dont-use-a-variable-for-two-unrelated-purposes)
  - [ES.27: Use `std::array` or `stack_array` for arrays on the stack](#es27-use-stdarray-or-stack_array-for-arrays-on-the-stack)
  - [ES.28: Use lambdas for complex initialization, especially of const variables](#es28-use-lambdas-for-complex-initialization-especially-of-const-variables)
  - [ES.30: Don't use macros for program text manipulation](#es30-dont-use-macros-for-program-text-manipulation)
  - [ES.31: Don't use macros for constants or "functions"](#es31-dont-use-macros-for-constants-or-functions)
  - [ES.32: Use ALL_CAPS for all macro names](#es32-use-all_caps-for-all-macro-names)
  - [ES.33: If you must use macros, give them unique names](#es33-if-you-must-use-macros-give-them-unique-names)
  - [ES.34: Don't define a (C-style) variadic function](#es34-dont-define-a-c-style-variadic-function)

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

## ES.21: Don't introduce a variable (or constant) before you need to use it
- Readability. To limit the scope in which the variable can be used.

## ES.22: Don't declare a variable until you have a value to initialize it with
- Readability. Limit the scope in which a variable can be used. Don't risk used-before-set.
- Initialization is often more efficient than assignment.
- For initializers of moderate complexity, including for const variables, consider using a lambda to express the initializer; see [ES.28](#es28-use-lambdas-for-complex-initialization-especially-of-const-variables).

## ES.23: Prefer the {}-initializer syntax
- Prefer `{}`. The rules for `{}` initialization are simpler, more general, less ambiguous, and safer than for other forms of initialization.
- Use = only when you are sure that there can be no narrowing conversions.
- For built-in arithmetic types, use = only with auto.
- Avoid `()` initialization, which allows parsing ambiguities.

```cpp
int x {f(99)};
int y = x;
vector<int> v = {1, 2, 3, 4, 5, 6};
```
- Exception: For containers, there is a tradition for using {...} for a list of elements and (...) for sizes:
```cpp
vector<int> v1(10);    // vector of 10 elements with the default value 0
vector<int> v2{10};    // vector of 1 element with the value 10

vector<int> v3(1, 2);  // vector of 1 element with the value 2
vector<int> v4{1, 2};  // vector of 2 element with the values 1 and 2
```
- Note: `{}`-initializers do not allow narrowing conversions (and that is usually a good thing) and allow explicit constructors (which is fine, we're intentionally initializing a new variable).

```cpp
int x {7.9};   // error: narrowing
int y = 7.9;   // OK: y becomes 7. Hope for a compiler warning
int z = gsl::narrow_cast<int>(7.9);  // OK: you asked for it
```
- Note: `{}` initialization can be used for nearly all initialization; other forms of initialization can't:
```cpp
auto p = new vector<int> {1, 2, 3, 4, 5};   // initialized vector
D::D(int a, int b) :m{a, b} {   // member initializer (e.g., m might be a pair)
    // ...
};
X var {};   // initialize var to be empty
struct S {
    int m {7};   // default initializer for a member
    // ...
};
```
- For that reason, `{}`-initialization is often called "uniform initialization" (though there unfortunately are a few irregularities left).
- Note: Initialization of a variable declared using `auto` with a single value, e.g., `{v}`, had surprising results until C++17. The C++17 rules are somewhat less surprising:
```cpp
auto x1 {7};        // x1 is an int with the value 7
auto x2 = {7};      // x2 is an initializer_list<int> with an element 7

auto x11 {7, 8};    // error: two initializers
auto x22 = {7, 8};  // x22 is an initializer_list<int> with elements 7 and 8
```
- Use `={...}` if you really want an `initializer_list<T>`
```cpp
auto fib10 = {1, 1, 2, 3, 5, 8, 13, 21, 34, 55};   // fib10 is a list
```
- Note: `={}` gives copy initialization whereas `{}` gives direct initialization.
- Like the distinction between copy-initialization and direct-initialization itself, this can lead to surprises.
  - `{}` accepts `explicit` constructors; `={}` does not. For example:

```cpp
struct Z { explicit Z() {} };
Z z1{};     // OK: direct initialization, so we use explicit constructor
Z z2 = {};  // error: copy initialization, so we cannot use the explicit constructor
```
- Use plain `{}`-initialization unless you specifically want to disable `explicit` constructors.

```cpp
template <typename T> void f() {
    T x1(1); // T initialized with 1
    T x0();  // bad: function declaration (often a mistake)

    T y1{1}; // T initialized with 1
    T y0{};  // default initialized T
    // ...
}
```

## ES.24: Use a `unique_ptr<T>` to hold pointers
- Using `std::unique_ptr` is the simplest way to avoid leaks. It is reliable, it makes the type system do much of the work to validate ownership safety, it increases readability, and it has zero or near zero run-time cost.

```cpp
void use(bool leak) {
    auto p1 = make_unique<int>(7); // OK
    int* p2 = new int{7};          // bad: might leak
    // ... no assignment to p2 ...
    if (leak)
        return;
    // ... no assignment to p2 ...
    vector<int> v(7);
    v.at(7) = 0; // exception thrown, which leak p2 here
    // ...
}
```
- If leak == true the object pointed to by `p2` is leaked and the object pointed to by `p1` is not. The same is the case when `at()` throws.

## ES.25: Declare an object `const` or `constexpr` unless you want to modify its value later on
- That way you can't change the value by mistake. That way might offer the compiler optimization opportunities.

```cpp
void f(int n) {
    const int bufmax = 2 * n + 2; // good: we can't change bufmax by accident
    int xmax = n;                 // suspicious: is xmax intended to change?
    // ...
}
```

## ES.26: Don't use a variable for two unrelated purposes
- Readability and safety.

```cpp
void use()
{
    int i;
    for (i = 0; i < 20; ++i) { /* ... */ }
    for (i = 0; i < 200; ++i) { /* ... */ } // bad: i recycled
}
```
- Note : As an optimization, you might want to reuse a buffer as a scratch pad, but even then prefer to limit the variable's scope as much as possible and be careful not to cause bugs from data left in a recycled buffer as this is a common source of security bugs.
```cpp
void write_to_file() {
    std::string buffer; // to avoid reallocations on every loop iteration
    for (auto& o : objects) {
        // First part of the work.
        generate_first_string(buffer, o);
        write_to_file(buffer);

        // Second part of the work.
        generate_second_string(buffer, o);
        write_to_file(buffer);

        // etc...
    }
}
```

## ES.27: Use `std::array` or `stack_array` for arrays on the stack
- They are readable and don't implicitly convert to pointers. They are not confused with non-standard extensions of built-in arrays.

```cpp
const int n = 7;
int m = 9;

void f() {
    int a1[n];
    int a2[m]; // error: not ISO C++, C syntax..
    // ...
}
```
- Note: The definition of `a1` is legal C++ and has always been. There is a lot of such code.
  - It is error-prone, though, especially when the bound is non-local.
  - Also, it is a "popular" source of errors (buffer overflow, pointers from array decay, etc.).
- The definition of a2 is C but not C++ and is considered a security risk
```cpp
// prefer:
const int n = 7;
int m = 9;
void f()
{
    array<int, n> a1;
    stack_array<int> a2(m);
    // ...
}
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

## ES.30: Don't use macros for program text manipulation
- Macros are a major source of bugs. Macros don't obey the usual scope and type rules.
- Macros ensure that the human reader sees something different from what the compiler sees.
- Macros complicate tool building.

```cpp
#define Case break; case   /* BAD */
```
- This innocuous-looking macro makes a single lower case c instead of a C into a bad flow-control bug.
- This rule does not ban the use of macros for "configuration control" use in #ifdefs, etc.
- In the future, modules are likely to eliminate the need for macros in configuration control.
- This rule is meant to also **discourage use of # for stringification and ## for concatenation**. As usual for macros, there are uses that are "mostly harmless", but even these can **create problems for tools, such as auto completers, static analyzers, and debuggers.** 
- Often the desire to use fancy macros is a sign of an overly complex design.
- Also, # and ## encourages the definition and use of macros:

```cpp
#define CAT(a, b) a##b
#define STRINGIFY(a) #a

void f(int x, int y) {
    string CAT(x, y) = "asdf"; // BAD: hard for tools to handle (and ugly)
    string sx2 = STRINGIFY(x);
    // ...
}
```
- There are workarounds for low-level string manipulation using macros. For example:

```cpp
string s = "asdf" "lkjh"; // ordinary string literal concatenation

enum E { a, b };

template <int x>
constexpr const char* stringify() {
    switch (x) {
    case a:
        return "a";
    case b:
        return "b";
    }
}

void f(int x, int y) {
    string sx = stringify<x>();
    // ...
}
```
- This is not as convenient as a macro to define, but as easy to use, has zero overhead, and is typed and scoped.
- In the future, static reflection is likely to eliminate the last needs for the preprocessor for program text manipulation.


## ES.31: Don't use macros for constants or "functions"
- Macros are a major source of bugs.
  - Macros don't obey the usual scope and type rules.
  - Macros don't obey the usual rules for argument passing.
  - Macros ensure that the human reader sees something different from what the compiler sees.
  - Macros complicate tool building.

```cpp
#define PI 3.14
#define SQUARE(a, b) (a * b)
```
- Even if we hadn't left a well-known bug in SQUARE there are much better behaved alternatives; for example:
```cpp
constexpr double pi = 3.14;
template<typename T> T square(T a, T b) { return a * b; }
```

## ES.32: Use ALL_CAPS for all macro names
- Convention. Readability. Distinguishing macros.

```cpp
Example
#define forever for (;;)   /* very BAD */
#define FOREVER for (;;)   /* Still evil, but at least visible to humans */
```

## ES.33: If you must use macros, give them unique names
- Macros do not obey scope rules.

```cpp
#define MYCHAR        /* BAD, will eventually clash with someone else's MYCHAR*/
#define ZCORP_CHAR    /* Still evil, but less likely to clash */
```
- Note: Avoid macros if you can: ES.30, ES.31, and ES.32.
- However, there are billions of lines of code littered with macros and a long tradition for using and overusing macros. If you are forced to use macros, use long names and supposedly unique prefixes (e.g., your organization's name) to lower the likelihood of a clash.

## ES.34: Don't define a (C-style) variadic function
- Not type safe. Requires messy cast-and-macro-laden code to get working right.

```cpp
#include <cstdarg>

// "severity" followed by a zero-terminated list of char*s; write the C-style
// strings to cerr
void error(int severity...) {
    va_list ap; // a magic type for holding arguments
    va_start(
        ap,
        severity); // arg startup: "severity" is the first argument of error()

    for (;;) {
        // treat the next var as a char*; no checking: a cast in disguise
        char* p = va_arg(ap, char*);
        if (!p)
            break;
        cerr << p << ' ';
    }

    va_end(ap); // arg cleanup (don't forget this)

    cerr << '\n';
    if (severity)
        exit(severity);
}

void use() {
    error(7, "this", "is", "an", "error", nullptr);
    error(7);                              // crash
    error(7, "this", "is", "an", "error"); // crash
    const char* is = "is";
    string an = "an";
    error(7, "this", "is", an, "error"); // crash
}
```
- Alternative: Overloading. Templates. Variadic templates.

```cpp
#include <iostream>

void error(int severity) {
    std::cerr << '\n';
    std::exit(severity);
}

template <typename T, typename... Ts>
constexpr void error(int severity, T head, Ts... tail) {
    std::cerr << head;
    error(severity, tail...);
}

void use() {
    error(7);                                     // No crash!
    error(5, "this", "is", "not", "an", "error"); // No crash!

    std::string an = "an";
    error(7, "this", "is", "not", an, "error"); // No crash!

    error(5, "oh", "no", nullptr); // Compile error! No need for nullptr.
}
```
- Note: This is basically the way printf is implemented.
