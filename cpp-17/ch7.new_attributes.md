# [C++17 - The complete guide, Ch 7:　New Attributes and Attribute Features](http://www.cppstd17.com/index.html)


- Since C++11, it has been possible to specify attributes (formal annotations that enable or disable warnings).
- With C++17, new attributes were introduced. In addition, attributes can now be used in a few more places and with some additional convenience.

## 7.1 Attribute `[[nodiscard]]`

- The new attribute `[[nodiscard]]` can be used to **encourage** warnings by the compiler if a return value of a function is not used (however, the **compiler is not required to issue a warning**).
- `[[nodiscard]]` should usually be used to signal misbehavior when return values are not used. The misbehavior might be:
  - Memory leaks, such as not using returned allocated memory
  - Unexpected or non-intuitive behavior such as getting different/unexpected behavior when not using the return value
  - Unnecessary overhead, such as calling something that is a no-op if the return value is not used
- Here are some examples of where using this attribute is useful:
  - Functions that allocate resources that have to be released by another function call should be marked with `[[nodiscard]]`.
  - A typical example would be a function to allocate memory, such as `malloc()` or the member function `allocate()` of allocators.
- However, note that some functions might return a value meaning that no compensating call is necessary.
  - For example, programmers call the C function `realloc()` with a size of zero bytes to free memory so that the return values do not have to be saved to call free() later.
  - For this reason, marking realloc() with `[[nodiscard]]` would be counterproductive.
- A good example of a function changing its behavior non-intuitively when not using the return value is `std::async()` (introduced with C++11).
  - `std::async()` starts a functionality asynchronously in the background and returns a handle that can be used to wait for the end of the started functionality (and get access to any return value or exception).
  - However, **when the return value is not used the call becomes a synchronous call because the destructor of the unused return value is called immediately waiting for the end of the started functionality.**
  - Therefore, not using the return value silently contradicts the whole purpose of calling `std::async()`. Compilers can warn about this when `std::async()` is marked with
`[[nodiscard]]`.
-  Another example is the member function `empty()`, which checks whether an object (container/string) has no elements.
  - Programmers pretty often call this function to “empty” the container (remove all elements)
  - This incorrect application of empty() can often be detected because it does not use the return value.
  - Therefore, marking the member function accordingly helps to detect such an logical error.

```cpp
class MyContainer {
...
public:
  [[nodiscard]] bool empty() const noexcept;
...
};
```

- If for whatever reason you do not want to use a return value marked with `[[nodiscard]]` you can cast the return value to `void`:

```cpp
(void)coll.empty(); // disable [[nodiscard]] warning
```

- Note that attributes in function declarations are not inherited if the functions are overwritten:

```cpp
struct B {
  [[nodiscard]] int* foo();
};
struct D : B {
  int* foo();
};
B b;
b.foo();        // warning
(void)b.foo();  // no warning
D d;
d.foo();  // no warning
```

- Therefore, you have mark the derived member function with `[[nodiscard]]` again (unless there is a reason that not using the return value makes sense in the derived class).
- As usual, you can place an attribute that applies to a function in front of all declaration specifiers or after the function name:

```cpp
class C {
  [[nodiscard]] friend bool operator== (const C&, const C&);
  friend bool operator!= [[nodiscard]] (const C&, const C&);
};
```

- Placing the attribute between `friend` and `bool` or between `bool` and `operator==` is not correct.
- Although the language feature was introduced with C++17, it is not used in the standard library yet. The proposal to apply this feature there simply came too late for C++17. Therefore, one of the key motivations for this feature, adding it to the declaration of `std::async()`, has not been done yet.
- However, for all the examples discussed above, corresponding fixes will come with the next C++ 20.
- However, to make your code more portable, you should use `[[nodiscard]]` instead of non-portable means (such as `[[gnu:warn_unused_result]]` for gcc and clang or `_Check_return_` for Visual C++) to mark functions accordingly.
- When defining operator `new()`, you should mark the functions with `[[nodiscard]]` as is done, for example, when defining a header file to track all calls of `new`.

## 7.2 Attribute `[[maybe_unused]]`

- The new attribute [[maybe_unused]] can be used to avoid warnings by the compiler for not using a name or entity.
- The attribute may be applied to the declaration of a class, a type definition with typedef or using, a variable, a non-static data member, a function, an enumeration type, or an enumerator (enumeration value).

- One application is to name a parameter without (necessarily) using it:

```cpp
void foo(int val, [[maybe_unused]] std::string msg) {
#ifdef DEBUG log(msg);
#endif
  ...
}
```

- Another example would be to have a member without using it:

```cpp
class MyStruct {
  char c;
  int i;
  [[maybe_unused]] char makeLargerSize[100];
  ...
};
```

- Note that you cannot apply `[[maybe_unused]]` to a statement. Therefore, you cannot counter `[[nodiscard]]` with `[[maybe_unused]]` directly:

```cpp
[[nodiscard]] void* foo();
int main() {
  foo();                            // WARNING: return value not used
  [[maybe_unused]] foo();           // ERROR: attribute not allowed here
  [[maybe_unused]] auto x = foo();  // OK
}
```

## 7.3 Attribute `[[fallthrough]]`

- The new attribute `[[fallthrough]]` can be used to avoid warnings by the compiler for not having a break statement after a sequence of one or more case labels inside a switch statement. For example:

```cpp
void commentPlace(int place) {
  switch (place) {
    case 1:
      std::cout << "very ";
      [[fallthrough]];
    case 2:
      std::cout << "well\n";
      break;
    default:
      std::cout << "OK\n";
      break;
  }
}
```


- Note that the attribute has to be used in **an empty statement**. It must therefore **end with a semicolon.**
- Using the attribute as the last statement in a switch statement is not allowed.

## 7.4 General Attribute Extensions

The following features were enabled for attributes in general with C++17:

### Attributes can now be used to mark namespaces.

- For example, you can now deprecate a namespace as follows:

```cpp
namespace [[deprecated]] DraftAPI {
...
}
```

- This is also possible for inline and unnamed namespaces.


### Attributes can now be used to mark enumerators (values of enumeration types).

- For example, you can introduce a new enumeration value as a replacement of an existing (now deprecated) enumeration value as follows:

```cpp
enum class City {
  Berlin = 0,
  NewYork = 1,
  Mumbai = 2,
  Bombay [[deprecated]] = Mumbai,
  ...
};
```

- Here, both `Mumbai` and `Bombay` represent the same numeric code for a city but using Bombay is marked as deprecated. Note that for enumeration values, the **attribute is placed behind the identifier**.

### User-defined attributes

- For user-defined attributes, which should usually be defined in their own namespace, you can now use a using prefix to avoid the repetition of the attribute namespace for each attribute. That is, instead of:

```cpp
[[MyLib::WebService, MyLib::RestService, MyLib::doc("html")]] void foo();
// all specified with MyLib::
```

you can just write

```cpp
[[using MyLib: WebService, RestService, doc("html")]] void foo();
// use using MyLib: once
```

Note that with a using prefix, using the namespace again is an error:

```cpp
[[using MyLib: MyLib::doc("html")]] void foo(); // ERROR
```