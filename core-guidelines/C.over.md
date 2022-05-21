# C.over: Overloading and overloaded operator
- You can overload ordinary functions, function templates, and operators. You cannot overload function objects.

## C.160: Define operators primarily to mimic conventional usage
- Minimize surprises.
- **Non-member operators should be either friends or defined in the same namespace as their operands.** - [C.168](#c168-define-overloaded-operators-in-the-namespace-of-their-operands)
- **Binary operators should treat their operands equivalently.**
```cpp
// Here, the conventional semantics is maintained: Copies compare equal.

class X {
  public:
    // ...
    X& operator=(const X&); // member function defining assignment
    friend bool operator==(const X&,
                           const X&); // == needs access to representation
                                      // after a = b we have a == b
    // ...
};
```

## C.161: Use non-member functions for symmetric operators
- If you use member functions, you need two.
- Unless you use a non-member function for (say) `==`, `a == b` and `b == a` will be subtly different.
```cpp
class X {
  public:
    X(int in) : x(in) {}
    bool operator==(const X& x2) { return x == x2.x; }
  private:
    int x;
};

class Y {
  public:
    Y(int in) : y(in) {}
    // difference: non-member operator== for symmetric ops
    friend bool operator==(const Y& y1, const Y& y2);

  private:
    int y;
};
bool operator==(const Y& y1, const Y& y2) { return y1.y == y2.y; }

//...
X x{1};
std::cout << (x == 2) << '\n';    // ok
// std::cout << (2 == X) << '\n'; // failed to compile
Y y{1};
std::cout << (y == 2) << '\n'; // ok
std::cout << (2 == y) << '\n'; // ok
```

## C.162: Overload operations that are roughly equivalent
- Having different names for logically equivalent operations on different argument types is confusing, leads to encoding type information in function names, and inhibits generic programming.

## C.163: Overload only for operations that are roughly equivalent
- Having the same name for logically different functions is confusing and leads to errors when using generic programming.
- Example, below two operations are fundamentally different (and unrelated) but if sharing the same names, to their (common) minimum, this opens opportunities for confusion.
```cpp
void open_gate(Gate& g);   // remove obstacle from garage exit lane
void fopen(const char* name, const char* mode);   // open file
```
- The two operations are fundamentally different (and unrelated) so it is good that their names differ. Conversely:
```cpp
void open(Gate& g);   // remove obstacle from garage exit lane
void open(const char* name, const char* mode ="r");   // open file .... bad!
```

## C.164: Avoid implicit conversion operators
- Implicit conversions can be essential (e.g., `double` to `int`) but often cause surprises (e.g., String to C-style string).
- Prefer explicitly named conversions until a serious need is demonstrated.
- By "serious need" we mean a reason that is fundamental in the application domain (such as an integer to complex number conversion) and frequently needed.
- Do not introduce implicit conversions (through conversion operators or non-explicit constructors) just to gain a minor convenience.
```cpp
struct S1 {
    string s;
    // ...
    operator char*() { return s.data(); } // BAD, likely to cause surprises
};

struct S2 {
    string s;
    // ...
    explicit operator char*() { return s.data(); }
};

void f(S1 s1, S2 s2) {
    char* x1 = s1; // OK, but can cause surprises in many contexts
    char* x2 = s2; // error (and that's usually a good thing)
    char* x3 = static_cast<char*>(s2); // we can be explicit (on your head be it)
}
```
- The surprising and potentially damaging implicit conversion can occur in arbitrarily hard-to spot contexts, e.g.,
```cpp
S1 ff();

char* g() {
    // The string returned by ff() is destroyed before the returned pointer into it can be used.
    return ff();
}
```

## C.165: Use `using` for customization points
- To find function objects and functions defined in a separate namespace to "customize" a common function.
- Consider `swap`. It is a general (standard-library) function with a definition that will work for just about any type. However, it is desirable to define specific `swap()`s for specific types.
- For example, the general `swap()` will copy the elements of two vectors being swapped, whereas a good specific implementation will not copy elements at all.
```cpp
namespace N {
My_type X{/* ... */};
void swap(X&, X&); // optimized swap for N::X
// ...
} // namespace N

void f1(N::X& a, N::X& b) {
    std::swap(a, b); // probably not what we wanted: calls std::swap()
}
```
- The `std::swap(`) in f`1()` does exactly what we asked it to do: it calls the `swap()` in namespace `std`. Unfortunately, that's probably not what we wanted. How do we get `N::X` considered?
```cpp
void f2(N::X& a, N::X& b) {
    swap(a, b);   // calls N::swap
}
```
- But that might not be what we wanted for generic code. There, we typically want the specific function if it exists and the general function if not. This is done by including the general function in the lookup for the function:
```cpp
void f3(N::X& a, N::X& b) {
    using std::swap;  // make std::swap available
    swap(a, b);        // calls N::swap if it exists, otherwise std::swap
}
```

## C.166: Overload unary `&` only as part of a system of smart pointers and references
- The `&` operator is fundamental in C++. Many parts of the C++ semantics assume its default meaning.
- If you "mess with" operator `&` be sure that its definition has matching meanings for `->`, `[]`, `*`, and `.` on the result type.
- Note that operator `.` currently cannot be overloaded so a perfect system is impossible.
```cpp
class Ptr {              // a somewhat smart pointer
    Ptr(X* pp) : p(pp) { /* check */
    }
    X* operator->() { /* check */
        return p;
    }
    X operator[](int i);
    X operator*();

  private:
    T* p;
};

class X {
    Ptr operator&() { return Ptr{this}; }
    // ...
};
```

## C.167: Use an operator for an operation with its conventional meaning
- Readability. Convention. Reusability. Support for generic code
```cpp
void cout_my_class(const My_class& c) // confusing, not conventional,not generic
{
    std::cout << c.some_member;
}

std::ostream& operator<<(std::ostream& os, const my_class& c) // OK
{
    return os << c.some_member;
}
```
- There are strong and vigorous conventions for the meaning of most operators, such as
  - comparisons (`==`, `!=`, `<`, `<=`, `>`, `>=`, and `<=>`),
  - arithmetic operations (`+`, `-`, `*`, `/`, and `%`)
  - access operations (`.`, `->`, `unary *`, and `[]`)
  - assignment (`=`)
- Don't define those unconventionally and don't invent your own names for them.

## C.168: Define overloaded operators in the namespace of their operands
- Readability.
- **Ability for find operators using ADL.**
- Avoiding inconsistent definition in different namespaces
```cpp
namespace N {
struct S { };
bool operator==(S, S);   // OK: in the same namespace as S, and even next to S
}

N::S s;
bool x = (s == s);  // finds N::operator==() by ADL
```
```cpp
// Here, the meaning of !s differs in namespace N and namespace M. This can be most confusing.
// Remove the definition of namespace M and the confusion is replaced by
// an opportunity to make the mistake.
struct S { };
S s;

namespace N {
    S::operator!(S a) { return true; }
    S not_s = !s;
}

namespace M {
    S::operator!(S a) { return false; }
    S not_s = !s;
}
```