# T.concepts: Concept rules

- [T.concepts: Concept rules](#tconcepts-concept-rules)
- [T.con-use: Concept use](#tcon-use-concept-use)
  - [T.10: Specify concepts for all template arguments](#t10-specify-concepts-for-all-template-arguments)
  - [T.11: Whenever possible use standard concepts](#t11-whenever-possible-use-standard-concepts)
  - [T.12: Prefer concept names over `auto` for local variables](#t12-prefer-concept-names-over-auto-for-local-variables)
  - [T.13: Prefer the shorthand notation for simple, single-type argument concepts](#t13-prefer-the-shorthand-notation-for-simple-single-type-argument-concepts)
- [T.concepts.def: Concept definition rules](#tconceptsdef-concept-definition-rules)
  - [T.20: Avoid "concepts" without meaningful semantics](#t20-avoid-concepts-without-meaningful-semantics)
  - [T.21: Require a complete set of operations for a concept](#t21-require-a-complete-set-of-operations-for-a-concept)
  - [T.22: Specify axioms for concepts](#t22-specify-axioms-for-concepts)
  - [T.23: Differentiate a refined concept from its more general case by adding new use patterns.](#t23-differentiate-a-refined-concept-from-its-more-general-case-by-adding-new-use-patterns)
  - [T.24: Use tag classes or traits to **differentiate concepts that differ only in semantics.**](#t24-use-tag-classes-or-traits-to-differentiate-concepts-that-differ-only-in-semantics)
  - [T.25: Avoid complementary constraints](#t25-avoid-complementary-constraints)
  - [T.26: Prefer to define concepts in terms of use-patterns rather than simple syntax](#t26-prefer-to-define-concepts-in-terms-of-use-patterns-rather-than-simple-syntax)


# T.con-use: Concept use

## T.10: Specify concepts for all template arguments
- Correctness and readability.
- The assumed meaning (syntax and semantics) of a template argument is fundamental to the interface of a template.
- A concept dramatically improves documentation and error handling for the template.
- Specifying concepts for template arguments is a powerful design tool.

```cpp
template <typename Iter, typename Val>
requires input_iterator<Iter> &&
    equality_comparable_with<iter_value_t<Iter>, Val>
        Iter find(Iter b, Iter e, Val v) {
    // ...
}
```
- or equivalently and more succinctly:
```cpp
template <input_iterator Iter, typename Val>
requires equality_comparable_with<iter_value_t<Iter>, Val>
    Iter find(Iter b, Iter e, Val v) {
    // ...
}
```
- Note: Plain `typename` (or `auto`) is the least constraining concept.
- It should be used only rarely when nothing more than "it's a type" can be assumed.
- This is typically only needed when (as part of template metaprogramming code) we manipulate pure expression trees, postponing type checking.

## T.11: Whenever possible use standard concepts
- "Standard" concepts (as provided by the GSL and the ISO standard itself) save us the work of thinking up our own concepts, are better thought out than we can manage to do in a hurry, and improve interoperability.
- Unless you are creating a new generic library, most of the concepts you need will already be defined by the standard library.

```cpp
template <typename T>
// don't define this: sortable is in <iterator>
concept Ordered_container =
    Sequence<T> && Random_access<Iterator<T>> && Ordered<Value_type<T>>;

void sort(Ordered_container auto& s);
```
- This `Ordered_container` is quite plausible, but it is very similar to the `sortable` concept in the standard library.
- Is it better? Is it right? Does it accurately reflect the standard's requirements for sort? It is better and simpler just to use `sortable`:
```cpp
void sort(sortable auto& s);   // better
```
- Note: The set of "standard" concepts is evolving as we approach an ISO standard including concepts.
- Note: Designing a useful concept is challenging.

## T.12: Prefer concept names over `auto` for local variables
- `auto` is the weakest concept.
- Concept names convey more meaning than just `auto`.

```cpp
vector<string> v{ "abc", "xyz" };
auto& x = v.front();        // bad
String auto& s = v.front(); // good (String is a GSL concept)
```

## T.13: Prefer the shorthand notation for simple, single-type argument concepts
- Readability. Direct expression of an idea.
- Example: To say "T is sortable":
```cpp
template <typename T> // Correct but verbose: "The parameter is
requires sortable<T>  // of type T which is the name of a type
void sort(T&);        // that is sortable"

template <sortable T> // Better: "The parameter is of type T
void sort(T&);        // which is Sortable"

void sort(sortable auto&); // Best: "The parameter is Sortable"
```
- The shorter versions better match the way we speak. Note that many templates don't need to use the `template` keyword.


# T.concepts.def: Concept definition rules
- Defining good concepts is non-trivial. Concepts are meant to represent fundamental concepts in an application domain (hence the name "concepts").
- Similarly throwing together a set of syntactic constraints to be used for the arguments for a single class or algorithm is not what concepts were designed for and will not give the full benefits of the mechanism.
- Obviously, defining concepts is most useful for code that can use an implementation (e.g., C++20 or later) but defining concepts is in itself a useful design technique and help catch conceptual errors and clean up the concepts (sic!) of an implementation.

## T.20: Avoid "concepts" without meaningful semantics
- Concepts are meant to express **semantic notions**, such as "a number", "a range" of elements, and "totally ordered."
- Simple constraints, such as "has a + operator" and "has a > operator" cannot be meaningfully specified in isolation and should be used only as building blocks for meaningful concepts, rather than in user code.

```cpp
template <typename T>
// bad; insufficient
concept Addable = requires(T a, T b) {
    a + b;
};

template <Addable N>
auto algo(const N& a, const N& b) // use two numbers
{
    // ...
    return a + b;
}

int x = 7;
int y = 9;
auto z = algo(x, y); // z = 16

string xx = "7";
string yy = "9";
auto zz = algo(xx, yy); // zz = "79"
```
- Maybe the concatenation was expected. More likely, it was an accident.
- Defining minus equivalently would give dramatically different sets of accepted types.
- This Addable violates the mathematical rule that addition is supposed to be commutative: a+b == b+a.
- Note: The ability to specify meaningful semantics is a defining characteristic of a true concept, as opposed to a syntactic constraint.

```cpp
template <typename T>
// The operators +, -, *, and / for a number are assumed to follow the usual
// mathematical rules
concept Number = requires(T a, T b) {
    a + b;
    a - b;
    a * b;
    a / b;
};

template <Number N> auto algo(const N& a, const N& b) {
    // ...
    return a + b;
}

int x = 7;
int y = 9;
auto z = algo(x, y); // z = 16

string xx = "7";
string yy = "9";
auto zz = algo(xx, yy); // error: string is not a Number
```
- Note: Concepts with multiple operations have far lower chance of accidentally matching a type than a single-operation concept.

## T.21: Require a complete set of operations for a concept
- Ease of comprehension. Improved interoperability. Helps implementers and maintainers.
- This is a specific variant of the general rule that a concept must make semantic sense.

```cpp
// Bad: This makes no semantic sense. You need at least + to make - meaningful and useful.
template <typename T>
concept Subtractable = requires(T a, T b) {
    a - b;
};
```
- Examples of complete sets are
  - Arithmetic: +, -, *, /, +=, -=, *=, /=
  - Comparable: <, >, <=, >=, ==, !=
- The rule supports the view that a concept should reflect a (mathematically) coherent set of operations.

- Note: This rule applies whether we use direct language support for concepts or not.
- It is a general design rule that even applies to non-templates:
  - This is minimal, but surprising and constraining for users. It could even be less efficient.
```cpp
class Minimal {
    // ...
};

bool operator==(const Minimal&, const Minimal&);
bool operator<(const Minimal&, const Minimal&);

Minimal operator+(const Minimal&, const Minimal&);
// no other operators

void f(const Minimal& x, const Minimal& y) {
    if (!(x == y)) { /* ... */
    }                // OK
    if (x != y) {    /* ... */
    }                // surprise! error

    while (!(x < y)) { /* ... */
    }                  // OK
    while (x >= y) {   /* ... */
    }                  // surprise! error

    x = x + y; // OK
    x += y;    // surprise! error
}
```

```cpp
class Convenient {
    // ...
};

bool operator==(const Convenient&, const Convenient&);
bool operator<(const Convenient&, const Convenient&);
// ... and the other comparison operators ...

Minimal operator+(const Convenient&, const Convenient&);
// ... and the other arithmetic operators ...

void f(const Convenient& x, const Convenient& y) {
    if (!(x == y)) { /* ... */
    }                // OK
    if (x != y) {    /* ... */
    }                // OK

    while (!(x < y)) { /* ... */
    }                  // OK
    while (x >= y) {   /* ... */
    }                  // OK

    x = x + y; // OK
    x += y;    // OK
}
```
- It can be a nuisance to define all operators, but not hard. Ideally, that rule should be language supported by giving you comparison operators by default.


## T.22: Specify axioms for concepts
- An **axiom in the mathematical sense: something that can be assumed without proof.**
- A meaningful/useful concept has a semantic meaning.
  - Expressing these semantics in an informal, semi-formal, or formal way makes the concept comprehensible to readers and the effort to express it can catch conceptual errors.
- Specifying semantics is a powerful design tool.

```cpp
template <typename T>
// The operators +, -, *, and / for a number are assumed to follow the usual
// mathematical rules
// axiom(T a, T b) {
//      a + b == b + a;
//      a - a == 0;
//      a * (b + c) == a * b + a * c;
//      //...
// }
concept Number = requires(T a, T b) {
    { a + b } -> convertible_to<T>;
    { a - b } -> convertible_to<T>;
    { a * b } -> convertible_to<T>;
    { a / b } -> convertible_to<T>;
};
```

- In general, axioms are not provable, and when they are the proof is often beyond the capability of a compiler. An axiom might not be general, but the template writer can assume that it holds for all inputs actually used (similar to a precondition).
- Note: In this context axioms are Boolean expressions.
  - Currently, C++ does not support axioms (even the ISO Concepts TS), so we have to make do with comments for a longish while.
  - Once language support is available, the // in front of the axiom can be removed
- Note: The GSL concepts have well-defined semantics; see the Palo Alto TR and the Ranges TS.
- Exception: Early versions of a new "concept" still under development will often just define simple sets of constraints without a well-specified semantics. -
- Finding good semantics can take effort and time. An incomplete set of constraints can still be very useful:

```cpp
// balancer for a generic binary tree
template <typename Node>
concept Balancer = requires(Node* p) {
    add_fixup(p);
    touch(p);
    detach(p);
};
```
- So a Balancer must supply at least these operations on a tree Node, but we are not yet ready to specify detailed semantics because a new kind of balanced tree might require more operations and the precise general semantics for all nodes is hard to pin down in the early stages of design.
- A "concept" that is incomplete or without a well-specified semantics can still be useful.
  - For example, it allows for some checking during initial experimentation.
  - However, it should not be assumed to be stable. Each new use case might require such an incomplete concept to be improved.


## T.23: Differentiate a refined concept from its more general case by adding new use patterns.
- Otherwise they cannot be distinguished automatically by the compiler.
- Example:
```cpp
template <typename I>
// Note: input_iterator is defined in <iterator>
concept Input_iter = requires(I iter) {
    ++iter;
};

template <typename I>
// Note: forward_iterator is defined in <iterator>
concept Fwd_iter = Input_iter<I> && requires(I iter) {
    iter++;
};
```
- The compiler can determine refinement based on the sets of required operations (here, suffix ++).
- This decreases the burden on implementers of these types since they do not need any special declarations to "hook into the concept".
- If two concepts have exactly the same requirements, they are logically equivalent (there is no refinement).


## T.24: Use tag classes or traits to **differentiate concepts that differ only in semantics.**
- Two concepts requiring the same syntax but having different semantics leads to ambiguity unless the programmer differentiates them.

```cpp
template <typename I> // iterator providing random access
// Note: random_access_iterator is defined in <iterator>
concept RA_iter = ...;

template <typename I> // iterator providing random access to contiguous data
// Note: contiguous_iterator is defined in <iterator>
concept Contiguous_iter =
    RA_iter<I> && is_contiguous_v<I>; // using is_contiguous trait
```
- The programmer (in a library) must define `is_contiguous` (a trait) appropriately.
- Wrapping a tag class into a concept leads to a simpler expression of this idea:

```cpp
template <typename I>
concept Contiguous = is_contiguous_v<I>;

template <typename I>
concept Contiguous_iter = RA_iter<I> && Contiguous<I>;
```
- Note: Traits can be trait classes or type traits. These can be user-defined or standard-library ones. Prefer the standard-library ones.
- The programmer (in a library) must define `is_contiguous` (a trait) appropriately.


## T.25: Avoid complementary constraints
- Clarity. Maintainability. Functions with complementary requirements expressed using negation are brittle.
- Example: Initially, people will try to define functions with complementary requirements:

```cpp
template <typename T>
requires !C<T> // bad
    void f();

template <typename T>
requires C<T>
void f();

// This is better:

template <typename T> // general template
void f();

template <typename T> // specialization by concept
requires C<T>
void f();
```
- The compiler will choose the unconstrained template only when `C<T>` is unsatisfied.
- If you do not want to (or cannot) define an unconstrained version of `f()`, then delete it.

```cpp
template<typename T>
void f() = delete;
```
- The compiler will select the overload, or emit an appropriate error.
- Note: Complementary constraints are unfortunately common in `enable_if` code:

```cpp
template <typename T>
enable_if<!C<T>, void> // bad
f();

template <typename T> enable_if<C<T>, void> f();
```
- Note: Complementary requirements on **one requirement is sometimes (wrongly) considered manageable**.
- However, for two or more requirements the number of definitions needs can go up exponentially (2,4,8,16,...) Now the opportunities for errors multiply....
```cpp
C1<T> && C2<T>
!C1<T> && C2<T>
C1<T> && !C2<T>
!C1<T> && !C2<T>
```

## T.26: Prefer to define concepts in terms of use-patterns rather than simple syntax
- The definition is more readable and corresponds directly to what a user has to write.
- Conversions are taken into account. You don't have to remember the names of all the type traits.

- Example: You might be tempted to define a concept Equality like this:
```cpp
template<typename T> concept Equality = has_equal<T> && has_not_equal<T>;
```
- Obviously, it would be better and easier just to use the standard `equality_comparable`, but - just as an example - if you had to define such a concept, prefer:
```cpp
template<typename T> concept Equality = requires(T a, T b) {
    { a == b } -> std::convertible_to<bool>;
    { a != b } -> std::convertible_to<bool>;
    // axiom { !(a == b) == (a != b) }
    // axiom { a = b; => a == b }  // => means "implies"
};
```
- as opposed to defining two meaningless concepts `has_equal` and `has_not_equal` just as helpers in the definition of Equality.
  - By "meaningless" we mean that we cannot specify the semantics of `has_equal` in isolation.
