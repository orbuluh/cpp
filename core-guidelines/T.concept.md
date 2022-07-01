# T.concepts: Concept rules

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

