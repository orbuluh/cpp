# T.interface Templates interfaces
- Over the years, programming with templates have suffered from a **weak distinction between the interface of a template and its implementation**.
- Before concepts, that distinction had no direct language support.
- However, the interface to a template is a critical concept - **a contract between a user and an implementer** - and should be carefully designed.

- [T.interface Templates interfaces](#tinterface-templates-interfaces)
  - [T.40: Use function objects to pass operations to algorithms](#t40-use-function-objects-to-pass-operations-to-algorithms)
  - [T.41: Require only essential properties in a template's concepts](#t41-require-only-essential-properties-in-a-templates-concepts)
  - [T.42: Use template aliases to simplify notation and hide implementation details](#t42-use-template-aliases-to-simplify-notation-and-hide-implementation-details)
  - [T.43: Prefer `using` over `typedef` for defining aliases](#t43-prefer-using-over-typedef-for-defining-aliases)
  - [T.44: Use function templates to deduce class template argument types (where feasible)](#t44-use-function-templates-to-deduce-class-template-argument-types-where-feasible)
  - [T.46: Require template arguments to be at least semi-regular](#t46-require-template-arguments-to-be-at-least-semi-regular)
  - [T.47: Avoid highly visible unconstrained templates with common names](#t47-avoid-highly-visible-unconstrained-templates-with-common-names)
  - [T.48: If your compiler does not support concepts, fake them with enable_if](#t48-if-your-compiler-does-not-support-concepts-fake-them-with-enable_if)
  - [T.49: Where possible, avoid type-erasure](#t49-where-possible-avoid-type-erasure)

## T.40: Use function objects to pass operations to algorithms
- Function objects can carry more information through an interface than a "plain" pointer to function.
- In general, passing function objects gives better performance than passing pointers to functions.

```cpp
bool greater(double x, double y) { return x > y; }
sort(v, greater);                                    // pointer to function: potentially slow
sort(v, [](double x, double y) { return x > y; });   // function object
sort(v, std::greater{});                             // function object

bool greater_than_7(double x) { return x > 7; }
auto x = find_if(v, greater_than_7);                 // pointer to function: inflexible
auto y = find_if(v, [](double x) { return x > 7; }); // function object: carries the needed data
auto z = find_if(v, Greater_than<double>(7));        // function object: carries the needed data

//You can, of course, generalize those functions using auto or concepts. For example:
auto y1 = find_if(v, [](totally_ordered auto x) { return x > 7; }); // require an ordered type
auto z1 = find_if(v, [](auto x) { return x > 7; });                 // hope that the type has a >
```
- Note: Lambdas generate function objects.
- Note: The performance argument dep

## T.41: Require only essential properties in a template's concepts
- Keep interfaces simple and stable.
- Example: Consider, a sort instrumented with (oversimplified) simple debug support:
```cpp
void sort(sortable auto& s) // sort sequence s
{
    if (debug)
        cerr << "enter sort( " << s << ")\n";
    // ...
    if (debug)
        cerr << "exit sort( " << s << ")\n";
}
```
- Should this be rewritten to:
```cpp
template <sortable S>
requires Streamable<S>
void sort(S& s) // sort sequence s
{
    if (debug)
        cerr << "enter sort( " << s << ")\n";
    // ...
    if (debug)
        cerr << "exit sort( " << s << ")\n";
}
```
- After all, there is nothing in sortable that requires iostream support.
- On the other hand, there is nothing in the fundamental idea of sorting that says anything about debugging.
- Note: If we require every operation used to be listed among the requirements, the interface becomes unstable: Every time we change the debug facilities, the usage data gathering, testing support, error reporting, etc., the definition of the template would need change and every use of the template would have to be recompiled. This is cumbersome, and in some environments infeasible.
- Conversely, if we use an operation in the implementation that is not guaranteed by concept checking, we might get a late compile-time error.
- By not using concept checking for properties of a template argument that is not considered essential, we delay checking until instantiation time.
  - We consider this a worthwhile tradeoff.
- Note: using non-local, non-dependent names (such as debug and cerr) also introduces context dependencies that might lead to "mysterious" errors.
- Note: It can be hard to decide which properties of a type are essential and which are not.

## T.42: Use template aliases to simplify notation and hide implementation details
- Improved readability. Implementation hiding.
- Note that template aliases replace many uses of traits to compute a type. They can also be used to wrap a trait.
```cpp
//Example
template <typename T, size_t N> class Matrix {
    // ...
    using Iterator = typename std::vector<T>::iterator;
    // ...
};
```
- This saves the user of Matrix from having to know that its elements are stored in a vector and also saves the user from repeatedly typing typename `std::vector<T>::`.

```cpp
template <typename T> void user(T& c) {
    // ...
    typename container_traits<T>::value_type x; // bad, verbose
    // ...
}

template <typename T>
using Value_type = typename container_traits<T>::value_type;
// This saves the user of Value_type from having to know the technique used to
// implement value_types.

template <typename T> void user2(T& c) {
    // ...
    Value_type<T> x;
    // ...
}
```
- Note: A simple, common use could be expressed: **"Wrap traits!"**


## T.43: Prefer `using` over `typedef` for defining aliases
- Improved readability: With `using`, the new name comes first rather than being embedded somewhere in a declaration.
- Generality: `using` can be used for template aliases, whereas `typedef`s can't easily be templates.
- Uniformity: `using` is syntactically similar to `auto`.

```cpp
typedef int (*PFI)(int); // OK, but convoluted

using PFI2 = int (*)(int); // OK, preferred

template <typename T> typedef int (*PFT)(T); // error

template <typename T> using PFT2 = int (*)(T); // OK - using is like an auto here
```

## T.44: Use function templates to deduce class template argument types (where feasible)
- Writing the template argument types explicitly can be tedious and unnecessarily verbose.

```cpp
tuple<int, string, double> t1 = {1, "Hamlet", 3.14}; // explicit type
auto t2 = make_tuple(1, "Ophelia"s, 3.14);           // better; deduced type
```
- Note the use of the s suffix to ensure that the string is a `std::string`, rather than a C-style string.
- Note: Since you can trivially write a `make_T` function, so could the compiler. Thus, `make_T` functions might become redundant in the future.
- Exception: Sometimes there isn't a good way of getting the template arguments deduced and sometimes, you want to specify the arguments explicitly:
```cpp
vector<double> v = { 1, 2, 3, 7.9, 15.99 };
list<Record*> lst;
```
- Note: C++17 will make this rule redundant by allowing the template arguments to be deduced directly **from constructor arguments:** Template parameter deduction for constructors (Rev. 3).
  - For example:
```cpp
tuple t1 = {1, "Hamlet"s, 3.14}; // deduced: tuple<int, string, double> in C++17
```

## T.46: Require template arguments to be at least semi-regular
- Readability. Preventing surprises and errors. Most uses support that anyway.

```cpp
class X {
public:
    explicit X(int);
    X(const X&);            // copy
    X operator=(const X&);
    X(X&&) noexcept;        // move
    X& operator=(X&&) noexcept;
    ~X();
    // ... no more constructors ...
};

X x {1};              // fine
X y = x;              // fine
std::vector<X> v(10); // error: no default constructor
```
- Note: Semiregular **requires default constructible.**


## T.47: Avoid highly visible unconstrained templates with common names
- An unconstrained template argument is a perfect match for anything so **such a template can be preferred over more specific types that require minor conversions**.
- This is particularly annoying/dangerous when ADL is used. Common names make this problem more likely.

```cpp
namespace Bad {
struct S {
    int m;
};
template <typename T1, typename T2> bool operator==(T1, T2) {
    cout << "Bad\n";
    return true;
}
} // namespace Bad

namespace T0 {
bool operator==(int, Bad::S) {
    cout << "T0\n";
    return true;
} // compare to int

void test() {
    Bad::S bad{1};
    vector<int> v(10);
    bool b = 1 == bad; // calling T0::operator ==(int, Bad::S)
    bool b2 = v.size() == bad; // calling Bad::operator ==(Bad::S, Bad::S)
}
} // namespace T0
```
- This prints `T0` and `Bad`.
- Now the `==` in `Bad` was designed to cause trouble, but would you have spotted the problem in real code?
  - The problem is that `v.size()` returns an unsigned integer so that a conversion is needed to call the local `T0::==`; the `Bad::==` requires no conversions.
  - Realistic types, such as the standard-library iterators can be made to exhibit similar anti-social tendencies.
- Note: If an unconstrained template is defined in the same namespace as a type, that unconstrained template can be found by ADL (as happened in the example). That is, it is highly visible.
- Note: This rule should not be necessary, but the committee cannot agree to exclude unconstrained templates from ADL.
- Unfortunately this will get many false positives; the standard library violates this widely, by putting many unconstrained templates and types into the single namespace `std`.

## T.48: If your compiler does not support concepts, fake them with enable_if
- Because that's the best we can do without direct concept support. `enable_if` can be used to conditionally define functions and to select among a set of functions.

```cpp
template <typename T> enable_if_t<is_integral_v<T>> f(T v) {
    // ...
}

// Equivalent to:
template <Integral T> void f(T v) {
    // ...
}
```
- Note: Beware of complementary constraints. Faking concept overloading using `enable_if` sometimes forces us to use that error-prone design technique.

## T.49: Where possible, avoid type-erasure
- Type erasure incurs an extra level of indirection by hiding type information behind a separate compilation boundary.
- Exceptions: Type erasure is sometimes appropriate, such as for `std::function`.
