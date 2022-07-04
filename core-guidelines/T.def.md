# T.def: Template definitions
- This section focuses on what is specific to template implementation. In particular, it focuses on a template definition's dependence on its context.

## T.60: Minimize a template's context dependencies
- Eases understanding. Minimizes errors from unexpected dependencies. Eases tool creation.

```cpp
template <typename C> void sort(C& c) {
    std::sort(begin(c), end(c)); // necessary and useful dependency
}

template <typename Iter> Iter algo(Iter first, Iter last) {
    for (; first != last; ++first) {
        auto x = sqrt(*first); // potentially surprising dependency: which sqrt()?
        helper(first, x); // potentially surprising dependency:
                          // helper is chosen based on first and x
        TT var = 7;       // potentially surprising dependency: which TT?
    }
}
```
- Note: Templates typically appear in header files so their context dependencies are more vulnerable to `#include` order dependencies than functions in .cpp files.
- Note: Having a template operate only on its arguments would be one way of reducing the number of dependencies to a minimum, but that would generally be unmanageable.
- For example, algorithms usually use other algorithms and invoke operations that do not exclusively operate on arguments. And don't get us started on macros!

## T.61: Do not over-parameterize members (SCARY)
- A member that does not depend on a template parameter cannot be used except for a specific template argument.
- This limits use and typically increases code size.

```cpp
template <typename T, typename A = std::allocator<T>>
// requires Regular<T> && Allocator<A>
class List {
  public:
    struct Link { // does not depend on A
        T elem;
        Link* pre;
        Link* suc;
    };

    using iterator = Link*;

    iterator first() const { return head; }

    // ...
  private:
    Link* head;
};

List<int> lst1;
List<int, My_allocator> lst2;
```
- This looks innocent enough, but now Link formally depends on the allocator (even though it doesn't use the allocator).
- This forces redundant instantiations that can be surprisingly costly in some real-world scenarios.
- Typically, the solution is to **make what would have been a nested class non-local, with its own minimal set of template parameters**.

```cpp
template <typename T> struct Link {
    T elem;
    Link* pre;
    Link* suc;
};

template <typename T, typename A = std::allocator<T>>
// requires Regular<T> && Allocator<A>
class List2 {
  public:
    using iterator = Link<T>*;

    iterator first() const { return head; }

    // ...
  private:
    Link<T>* head;
};

List2<int> lst1;
List2<int, My_allocator> lst2;
```
- Some people found the idea that the Link no longer was hidden inside the list scary, so we named the technique SCARY.
- From that academic paper: "The acronym SCARY describes **assignments and initializations that are Seemingly erroneous** (appearing Constrained by conflicting generic parameters), but Actually work with the Right implementation (unconstrained bY the conflict due to minimized dependencies)."
- Note: This also applies to lambdas that don't depend on all of the template parameters.

## T.62: Place non-dependent class template members in a non-templated base class
- Allow the base class members to be used without specifying template arguments and without template instantiation.

```cpp
template <typename T> class Foo {
  public:
    enum { v1, v2 };
    // ...
};

struct Foo_base {
    enum { v1, v2 };
    // ...
};

template <typename T> class Foo : public Foo_base {
  public:
    // ...
};
```
- Note: A more general version of this rule would be "**If a class template member depends on only N template parameters out of M, place it in a base class with only N parameters.**"
- For N == 1, we have a choice of a base class of a class in the surrounding scope as in [T.61](#t61-do-not-over-parameterize-members-scary).

## T.64: Use specialization to provide alternative implementations of class templates
- A template defines a general interface.
- Specialization offers a powerful mechanism for providing alternative implementations of that interface.

## T.65: Use tag dispatch to provide alternative implementations of a function
- A template defines a general interface.
- Tag dispatch allows us to select implementations based on specific properties of an argument type.
- Performance.
- Example: This is a simplified version of std::copy (ignoring the possibility of non-contiguous sequences)
  - This is a general and powerful technique for compile-time algorithm selection.
```cpp
struct pod_tag {};
struct non_pod_tag {};

template <class T>
struct copy_trait {
    using tag = non_pod_tag;
}; // T is not "plain old data"

template <>
struct copy_trait<int> {
    using tag = pod_tag;
}; // int is "plain old data"

template <class Iter>
Out copy_helper(Iter first, Iter last, Iter out, pod_tag) {
    // use memmove
}

template <class Iter>
Out copy_helper(Iter first, Iter last, Iter out, non_pod_tag) {
    // use loop calling copy constructors
}

template <class Iter>
Out copy(Iter first, Iter last, Iter out) {
    return copy_helper(first, last, out, typename copy_trait<Iter>::tag{})
}

void use(vector<int>& vi, vector<int>& vi2, vector<string>& vs,
         vector<string>& vs2) {
    copy(vi.begin(), vi.end(), vi2.begin()); // uses memmove
    copy(vs.begin(), vs.end(), vs2.begin()); // uses a loop calling copy constructors
}
```
- Note: When concepts become widely available such alternatives can be distinguished directly:

```cpp
template <class Iter>
requires Pod<Value_type<iter>>
Out copy_helper(In, first, In last, Out out) {
    // use memmove
}

template <class Iter>
Out copy_helper(In, first, In last, Out out) {
    // use loop calling copy constructors
}
```

## T.67: Use specialization to provide alternative implementations for irregular types
- empty session

## T.68: Use {} rather than () within templates to avoid ambiguities
- `()` is vulnerable to grammar ambiguities.

```cpp
template<typename T, typename U>
void f(T t, U u)
{
    T v1(T(u));    // mistake: oops, v1 is a function not a variable
    T v2{u};       // clear:   obviously a variable
    auto x = T(u); // unclear: construction or cast?
}

f(1, "asdf"); // bad: cast from const char* to int
```

## T.69: Inside a template, don't make an unqualified non-member function call unless you intend it to be a customization point
- Provide only intended flexibility.
- Avoid vulnerability to accidental environmental changes.
- There are three major ways to let calling code customize a template.
```cpp
template <class T>
// Call a member function
void test1(T t) {
    t.f(); // require T to provide f()
}

template <class T>
void test2(T t)
// Call a non-member function without qualification
{
    f(t); // require f(/*T*/) be available in caller's scope or in T's namespace
}

template <class T>
void test3(T t)
// Invoke a "trait"
{
    test_traits<T>::f(t); // require customizing test_traits<>
                          // to get non-default functions/types
}
```
- A trait is usually a type alias to compute a type, a `constexpr` function to compute a value, or a traditional traits template to be specialized on the user's type.

- Note: If you intend to call your own helper function `helper(t)` with a value `t` that depends on a template type parameter, put it in a `::detail` namespace and qualify the call as `detail::helper(t);`.
- An unqualified call becomes a customization point where any function helper in the namespace of t's type can be invoked; this can cause problems like unintentionally invoking unconstrained function templates.
