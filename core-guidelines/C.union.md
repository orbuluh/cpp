# C.union: Unions
- A union is a struct where **all members start at the same address** so that it **can hold only one member at a time**.
- A union does not keep track of which member is stored so **the programmer has to get it right**; this is inherently error-prone, but there are ways to compensate.
- A type that is a **union plus an indicator of which member is currently held** is called a **tagged union**, a **discriminated union**, or a **`variant`**.

- [C.union: Unions](#cunion-unions)
  - [C.180: Use `union`s to save memory](#c180-use-unions-to-save-memory)
  - [C.181: Avoid "naked" `union`s](#c181-avoid-naked-unions)
  - [C.182: Use anonymous unions to implement tagged unions](#c182-use-anonymous-unions-to-implement-tagged-unions)
  - [C.183: Don't use a `union` for type punning](#c183-dont-use-a-union-for-type-punning)

## C.180: Use `union`s to save memory
- A `union` allows a single piece of memory to be used for different types of objects at different times. Consequently, it can be used to save memory when we have several objects that are never used at the same time.
```cpp
union Value {
    int x;
    double d;
};

Value v = {123};     // now v holds an int
cout << v.x << '\n'; // write 123
v.d = 987.654;       // now v holds a double
cout << v.d << '\n'; // write 987.654
```
```cpp
class Immutable_string { // Short-string optimization
    // Slightly larger than the size of a pointer (so we can use union)
  static constexpr size_t buffer_size = 16;
  public:
    Immutable_string(const char* str) : size(strlen(str)) {
        if (size < buffer_size)
            strcpy_s(string_buffer, buffer_size, str);
        else {
            string_ptr = new char[size + 1];
            strcpy_s(string_ptr, size + 1, str);
        }
    }

    ~Immutable_string() {
        if (size >= buffer_size)
            delete[] string_ptr;
    }

    const char* get_str() const {
        return (size < buffer_size) ? string_buffer : string_ptr;
    }

  private:
    // If the string is short enough, we store the string itself
    // instead of a pointer to the string.
    union {
        char* string_ptr;
        char string_buffer[buffer_size];
    };

    const size_t size;
};
```
## C.181: Avoid "naked" `union`s
- A naked union is a union without an associated indicator which member (if any) it holds, so that the programmer has to keep track. Naked unions are a source of type errors.
```cpp
union Value {
    int x;
    double d;
};

Value v;
v.d = 987.654; // v holds a double now
//...
cout << v.x << '\n'; // BAD, undefined behavior: v holds a double, but we read
                     // it as an int
```
- Note that the type error happened without any explicit cast. When we tested that program the last value printed was 1683627180 which is the integer value for the bit pattern for 987.654.
- What we have here is an **"invisible"** type error that happens to give a result that could easily look innocent.
- And, talking about "invisible", this code produced no output:
```cpp
v.x = 123;
cout << v.d << '\n';    // BAD: undefined behavior
```
- Alternative: Wrap a union in a class together with a type field. The C++17 `variant` type (found in `<variant>`) does that for you:
```cpp
variant<int, double> v;
v = 123;        // v holds an int
int x = get<int>(v);
v = 123.456;    // v holds a double
w = get<double>(v);
```

## C.182: Use anonymous unions to implement tagged unions
- A well-designed tagged union is type safe. An anonymous union simplifies the definition of a class with a (tag, union) pair.
- Handling a type with user-defined assignment and destructor is tricky. Saving programmers from having to write such code is one reason for including variant in the standard.
```cpp
class Value { // two alternative representations represented as a union
  private:
    enum class Tag { number, text };
    Tag type; // discriminant

    union { // representation (note: anonymous union)
        int i;
        string s; // string has default constructor, copy operations, and
                  // destructor
    };

  public:
    struct Bad_entry {}; // used for exceptions

    ~Value();
    Value& operator=(const Value&); // necessary because of the string variant
    Value(const Value&);
    // ...
    int number() const;
    string text() const;

    void set_number(int n);
    void set_text(const string&);
    // ...
};

int Value::number() const {
    if (type != Tag::number)
        throw Bad_entry{};
    return i;
}

string Value::text() const {
    if (type != Tag::text)
        throw Bad_entry{};
    return s;
}

void Value::set_number(int n) {
    if (type == Tag::text) {
        s.~string(); // explicitly destroy string
        type = Tag::number;
    }
    i = n;
}

void Value::set_text(const string& ss) {
    if (type == Tag::text)
        s = ss;
    else {
        new (&s) string{ss}; // placement new: explicitly construct string
        type = Tag::text;
    }
}

Value&
Value::operator=(const Value& e) // necessary because of the string variant
{
    if (type == Tag::text && e.type == Tag::text) {
        s = e.s; // usual string assignment
        return *this;
    }

    if (type == Tag::text)
        s.~string(); // explicit destroy

    switch (e.type) {
    case Tag::number:
        i = e.i;
        break;
    case Tag::text:
        new (&s) string(e.s); // placement new: explicit construct
    }

    type = e.type;
    return *this;
}

Value::~Value() {
    if (type == Tag::text)
        s.~string(); // explicit destroy
}
```

## C.183: Don't use a `union` for type punning
- It is undefined behavior to read a union member with a different type from the one with which it was written.
- Such punning is invisible, or at least harder to spot than using a named cast.
- Type punning using a union is a source of errors.
- Example, bad
```cpp
// The idea of Pun is to be able to look at the character representation of an int.
union Pun {
    int x;
    unsigned char c[sizeof(int)];
};
void bad(Pun& u) {
    u.x = 'x';
    cout << u.c[0] << '\n'; // undefined behavior
}
```
- If you wanted to see the bytes of an int, use a (named) cast.
  - Accessing the result of a `reinterpret_cast` from the object's declared type to `char*`, `unsigned char*`, or` std::byte* `is defined behavior.
  - (Using `reinterpret_cast` is discouraged, but at least we can see that something tricky is going on.)
```cpp
void if_you_must_pun(int& x) {
    auto p = reinterpret_cast<std::byte*>(&x);
    cout << p[0] << '\n';     // OK; better
    // ...
}
```
- Unfortunately, `unions` are commonly used for type punning. We don't consider "sometimes, it works as expected" a conclusive argument.
- C++17 introduced a distinct type `std::byte` to facilitate operations on raw object representation. Use that type instead of `unsigned char` or `char` for these operation.

