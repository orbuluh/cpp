# C.ctor: Constructors

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


TBC: ## C.43!














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

