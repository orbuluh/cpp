# C.ctor: Constructors

- [C.ctor: Constructors](#cctor-constructors)
  - [C.40: Define a constructor if a class has an invariant](#c40-define-a-constructor-if-a-class-has-an-invariant)
  - [C.41: A constructor should create a fully initialized object](#c41-a-constructor-should-create-a-fully-initialized-object)
  - [C.42: If a constructor cannot construct a valid object, throw an exception](#c42-if-a-constructor-cannot-construct-a-valid-object-throw-an-exception)
  - [C.43: Ensure that a copyable class has a default constructor](#c43-ensure-that-a-copyable-class-has-a-default-constructor)
  - [C.44: Prefer default constructors to be simple and non-throwing](#c44-prefer-default-constructors-to-be-simple-and-non-throwing)
  - [C.45: Don't define a default constructor that only initializes data members; use in-class member initializers instead](#c45-dont-define-a-default-constructor-that-only-initializes-data-members-use-in-class-member-initializers-instead)
  - [C.46: By default, declare single-argument constructors `explicit`](#c46-by-default-declare-single-argument-constructors-explicit)
  - [C.47: Define and initialize member variables in the order of member declaration](#c47-define-and-initialize-member-variables-in-the-order-of-member-declaration)
  - [C.48: Prefer in-class initializers to member initializers in constructors for constant initializers](#c48-prefer-in-class-initializers-to-member-initializers-in-constructors-for-constant-initializers)
  - [C.49: Prefer initialization to assignment in constructors](#c49-prefer-initialization-to-assignment-in-constructors)
  - [C.50: Use a factory function if you need "virtual behavior" during initialization](#c50-use-a-factory-function-if-you-need-virtual-behavior-during-initialization)
  - [C.51: Use delegating constructors to represent common actions for all constructors of a class](#c51-use-delegating-constructors-to-represent-common-actions-for-all-constructors-of-a-class)
  - [C.52: Use inheriting constructors to import constructors into a derived class that does not need further explicit initialization](#c52-use-inheriting-constructors-to-import-constructors-into-a-derived-class-that-does-not-need-further-explicit-initialization)

## C.40: Define a constructor if a class has an invariant
```cpp
class Date {  // a Date represents a valid date
              // in the January 1, 1900 to December 31, 2100 range
    Date(int dd, int mm, int yy)
        : d{dd}, m{mm}, y{yy}
    {
        if (!is_valid(d, m, y)) throw Bad_date{};  // enforce invariant
    }
    // ...
private:
    int d, m, y;
};
```

## C.41: A constructor should create a fully initialized object
- A user of a class should be able to assume that a constructed object is usable.
```cpp
class Bad {
    FILE* f;   // call init() before any other function
    // ...
public:
    Bad() {}
    void init();   // initialize f
    void read();   // read from f
    // ...
};

void f()
{
    Bad file;
    file.read();   // crash or bad read!
    // ...
    file.init();   // too late
    // ...
}
```
- Exception: If a valid object cannot conveniently be constructed by a constructor, use a factory function.

## C.42: If a constructor cannot construct a valid object, throw an exception
- Leaving behind an invalid object is asking for trouble.
- For a variable definition (e.g., on the stack or as a member of another object) there is no explicit function call from which an error code could be returned. Leaving behind an invalid object and relying on users to consistently check an is_valid() function before use is tedious, error-prone, and inefficient.
- If you feel tempted to use some "post-constructor initialization" or "two-stage initialization" idiom, try not to do that. If you really have to, look at factory functions.
- Exception: There are domains, such as some hard-real-time systems (think airplane controls) where (without additional tool support) exception handling is not sufficiently predictable from a timing perspective. There the is_valid() technique must be used. In such cases, check is_valid() consistently and immediately to simulate RAII.
```cpp
class Preferable {
    FILE* f;
    // ...
public:
    Preferable(const string& name)
        :f{fopen(name.c_str(), "r")}
    {
        if (!f) throw runtime_error{"could not open" + name};
        // ...
    }

    void read();      // read from f
    // ...
};

void f()
{
    Preferable file {"Zeno"}; // throws if file isn't open
    file.read();      // fine
    // ...
}
```
```cpp
class Bad {     // bad: the constructor leaves a non-valid object behind
    FILE* f;   // call is_valid() before any other function
    bool valid;
    // ...
public:
    Bad(const string& name)
        :f{fopen(name.c_str(), "r")}, valid{false}
    {
        if (f) valid = true;
        // ...
    }

    bool is_valid() { return valid; }
    void read();   // read from f
    // ...
};

void f()
{
    Bad file {"Heraclides"};
    file.read();   // crash or bad read!
    // ...
    if (file.is_valid()) {
        file.read();
        // ...
    }
    else {
        // ... handle error ...
    }
    // ...
}
```
- One reason people have used `init()` functions rather than doing the initialization work in a constructor has been to avoid code replication. Delegating constructors and default member initialization do that better. Another reason has been to delay initialization until an object is needed; the solution to that is often not to declare a variable until it can be properly initialized

## C.43: Ensure that a copyable class has a default constructor
- A default constructor often simplifies the task of defining a suitable moved-from state for a type that is also copyable.
- The absence of a default value can cause surprises for users and complicate its use, so if one can be reasonably defined, it should be.
```cpp
class Date { // BAD: no default constructor
public:
    Date(int dd, int mm, int yyyy);
    // ...
};

vector<Date> vd1(1000);   // default Date needed here
```
```cpp
class Date {
public:
    Date(int dd, int mm, int yyyy);
    Date() = default; // [See also](#Rc-default)
    // ...
private:
    // There is no "natural" default date. However, most realistic Date classes have
    // a "first date" (e.g. January 1, 1970 is popular), so making that the default is usually trivial.
    int dd = 1;
    int mm = 1;
    int yyyy = 1970;
    // ...
};

vector<Date> vd1(1000); // alright
```
- Beware that built-in types are not properly default constructed.
```cpp
struct X {
    string s;
    int i;
};

void f()
{
    X x;    // x.s is initialized to the empty string; x.i is uninitialized

    cout << x.s << ' ' << x.i << '\n';
}
```
- **Statically allocated objects of built-in types are by default initialized to 0, but local built-in variables are not.**
  - Beware that your compiler might default initialize local built-in variables, whereas an optimized build will not.
  - Thus, code like the example above might appear to work, but it relies on undefined behavior. Assuming that you want initialization, an explicit default initialization can help:
```
struct X {
    string s;
    int i {};   // default initialize (to 0)
};
```
- Classes that don't have a reasonable default construction are usually not copyable either, so they don't fall under this guideline.
  - For example, a base class should not be copyable, and so does not necessarily need a default constructor:
```cpp
struct Shape {
    virtual void draw() = 0;
    virtual void rotate(int) = 0;
    // =delete copy/move functions
    // ...
};
```
- A class that must acquire a caller-provided resource during construction often cannot have a default constructor, but it does not fall under this guideline because such a class is usually not copyable anyway:
```cpp
// std::lock_guard is not a copyable type.
// It does not have a default constructor.
lock_guard g {mx};  // guard the mutex mx
lock_guard g2;      // error: guarding nothing
```
- A class that has a "special state" that must be handled separately from other states by member functions or users causes extra work (and most likely more errors). Such a type can naturally use the special state as a default constructed value, whether or not it is copyable:
```cpp
// std::ofstream is not a copyable type.
// It does happen to have a default constructor
// that goes along with a special "not open" state.
ofstream out {"Foobar"};
// ...
out << log(time, transaction);
```
- Similar special-state types that are copyable, such as copyable smart pointers that have the special state "==nullptr", should use the special state as their default constructed value.
- However, it is preferable to have a default constructor default to a meaningful state such as `std::strings ""` and `std::vectors {}`.

## C.44: Prefer default constructors to be simple and non-throwing
- Being able to set a value to "the default" without operations that might fail simplifies error handling and reasoning about move operations.

## C.45: Don't define a default constructor that only initializes data members; use in-class member initializers instead
- Using in-class member initializers lets the compiler generate the function for you. The compiler-generated function can be more efficient.
```cpp
class Bad { // BAD: doesn't use member initializers
    string s;
    int i;
public:
    Bad() :s{"default"}, i{1} { }
    // ...
};
```
```cpp
class Preferable {
    string s = "default";
    int i = 1;
public:
    // use compiler-generated default constructor
    // ...
};
```

## C.46: By default, declare single-argument constructors `explicit`
- To avoid unintended conversions.
```cpp
class BadString {
public:
    BadString(int);   // BAD
    // ...
};

BadString s = 10;   // surprise: string of size 10
```
- Exception - unless you really want an implicit conversion from the constructor argument type to the class type
```cpp
class Complex {
public:
    Complex(double d);   // OK: we want a conversion from d to {d, 0}
    // ...
};

Complex z = 10.7;   // expected conversion
```
- Copy and move constructors should not be made explicit because they do not perform conversions. Explicit copy/move constructors make passing and returning by value difficult.

## C.47: Define and initialize member variables in the order of member declaration
- To minimize confusion and errors. That is the order in which the initialization happens (**independent of the order of member initializers**).

## C.48: Prefer in-class initializers to member initializers in constructors for constant initializers
- Makes it explicit that the same value is expected to be used in all constructors.
- Avoids repetition. Avoids maintenance problems. It leads to the shortest and most efficient code.
```cpp
class PreferWay {
    int i {666};
    string s {"qqq"};
    int j {0};
public:
    PreferWay() = default;        // all members are initialized to their defaults
    PreferWay(int ii) :i{ii} {}   // s and j initialized to their defaults
    // ...
};
```
```cpp
class Bad {
    int i;
    string s;
    int j;
public:
    Bad() :i{666}, s{"qqq"} { }   // j is uninitialized
    Bad(int ii) :i{ii} {}         // s is "" and j is uninitialized
    // ...
};
```
```cpp
class AlsoBad {   // BAD: inexplicit, argument passing overhead
    int i;
    string s;
    int j;
public:
    AlsoBad(int ii = 666, const string& ss = "qqq", int jj = 0)
        :i{ii}, s{ss}, j{jj} { }   // all members are initialized to their defaults
    // ...
};
```

## C.49: Prefer initialization to assignment in constructors
- An initialization explicitly states that initialization, rather than assignment, is done and can be more elegant and efficient. Prevents "use before set" errors.
```cpp
class A {   // Good
    string s1;
public:
    A(czstring p) : s1{p} { }    // GOOD: directly construct (and the C-string is explicitly named)
    // ...
};
```
```cpp
class B {   // BAD
    string s1;
public:
    B(const char* p) { s1 = p; }   // BAD: default constructor followed by assignment
    // ...
};
```

## C.50: Use a factory function if you need "virtual behavior" during initialization
- If the state of a base class object must depend on the state of a derived part of the object, we need to use a virtual function (or equivalent) while minimizing the window of opportunity to misuse an imperfectly constructed object.
- The return type of the factory should normally be `unique_ptr` by default; if some uses are `shared`, the caller can move the `unique_ptr` into a `shared_ptr`.
- However, if the factory author knows that all uses of the returned object will be shared uses, return `shared_ptr` and use `make_shared` in the body to save an allocation.
```cpp
class Bad {
public:
    Bad()
    {
        /* ... */
        f(); // BAD: C.82: Don't call virtual functions in constructors and destructors
        /* ... */
    }

    virtual void f() = 0;
};
```
```cpp
class B {
  protected:
    class Token {};

  public:
    explicit B(Token) { /* ... */
    }                   // create an imperfectly initialized object
    virtual void f() = 0;

    template <class T>
    static shared_ptr<T> create() // interface for creating shared objects
    {
        auto p = make_shared<T>(typename T::Token{});
        p->post_initialize();
        return p;
    }

  protected:
    virtual void post_initialize() // called right after construction
    {                              /* ... */
        f();                       /* ... */
    }                              // GOOD: virtual dispatch is safe
};

class D : public B { // some derived class
  protected:
    class Token {};

  public:
    explicit D(Token) : B{B::Token{}} {}
    void f() override{/* ...  */};

  protected:
    template <class T> friend shared_ptr<T> B::create();
};

shared_ptr<D> p = D::create<D>(); // creating a D object
```
- `make_shared` requires that the constructor is public. **By requiring a `protected` Token the constructor cannot be publicly called anymore**, so we avoid an incompletely constructed object escaping into the wild.
- By providing the factory function `create()`, we make construction (on the free store) convenient.

## C.51: Use delegating constructors to represent common actions for all constructors of a class
```cpp
class DateBad {   // BAD: repetitive
    int d;
    Month m;
    int y;
public:
    DateBad(int dd, Month mm, year yy)
        :d{dd}, m{mm}, y{yy}
        { if (!valid(d, m, y)) throw Bad_date{}; }

    DateBad(int dd, Month mm)
        :d{dd}, m{mm} y{current_year()}
        { if (!valid(d, m, y)) throw Bad_date{}; }
    // ...
};
```
```cpp
class DatePreferable {
    int d;
    Month m;
    int y;
public:
    DatePreferable(int dd, Month mm, year yy)
        :d{dd}, m{mm}, y{yy}
        { if (!valid(d, m, y)) throw Bad_date{}; }

    DatePreferable(int dd, Month mm)
        :DatePreferable{dd, mm, current_year()} {}
    // ...
};
```

## C.52: Use inheriting constructors to import constructors into a derived class that does not need further explicit initialization
- If you need those constructors for a derived class, re-implementing them is tedious and error-prone.
```cpp
class Rec {
    // ... data and lots of nice constructors ...
};

class Oper : public Rec {
    using Rec::Rec;
    // ... no data members ...
    // ... lots of nice utility functions ...
};
```
