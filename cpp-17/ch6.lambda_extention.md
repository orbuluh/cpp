# [C++17 - The complete guide, Ch 6: Lambda Extensions](http://www.cppstd17.com/index.html)

## 6.1 `constexpr` Lambdas

- Since C++17, lambdas are implicitly `constexpr` if possible. That is, **any** lambda can be used in compiletime `contexts` provided the features it uses are valid **for compile-time contexts** (e.g., only literal types, no static variables, no virtual, no try/catch, no new/delete).
  - Only literal types:
    - In compile-time contexts, only literal types are allowed. Literal types are types whose values are known at compile time and can be explicitly represented in the source code.
    - Examples of literal types include integers, floating-point numbers, booleans, characters, and string literals.
    - This restriction ensures that the code can be evaluated and resolved at compile time without relying on runtime information.

  - No `static` variables:
    - Static variables are variables that are shared among all instances of a class or within a specific scope.
    - **In compile-time contexts, static variables are not allowed because their initialization and access involve runtime operations.**
    - Since compile-time contexts focus on evaluating code during compilation, they do not support runtime-dependent features like static variables.

  - No `virtual`:
    - The `virtual` keyword is used for defining and using virtual functions or methods, which are resolved at runtime based on the actual type of an object.
    - Compile-time contexts do not support virtual functions because they require runtime polymorphism, which conflicts with the goal of evaluating code during compilation.

  - No `try`/`catch`:
    - The try/catch construct is used for exception handling in many programming languages. It allows **catching and handling exceptions that occur during runtime.**
    - Compile-time contexts do not permit try/catch because they are concerned with the static analysis of code, not the handling of runtime exceptions.

  - No `new`/`delete`:
    - The `new` and `delete` keywords are typically used for dynamic memory allocation and deallocation in programming languages
    - Since compile-time contexts operate during compilation and focus on static analysis, they do not support dynamic memory management operations like `new` and `delete`, which are performed at runtime.

```cpp
auto squared = [](auto val) {  // implicitly constexpr since C++17
  return val * val;
};
std::array<int, squared(5)> a;  // OK since C++17 => std::array<int,25>

auto squared2 = [](auto val) {  // implicitly constexpr since C++17
  static int calls = 0;  // OK, but disables lambda for constexpr contexts
  ... return val * val;
};
std::array<int, squared2(5)> a;  // ERROR: static variable in compile-time context
std::cout << squared2(5) << '\n';  // OK
```

- To find out at compile time whether a lambda is valid for a compile-time context, you can declare it as below

```cpp
auto squared3 = [](auto val) constexpr {  // OK since C++17
  return val * val;
};

auto squared3i = [](int val) constexpr -> int {  // OK since C++17
  return val * val;
};
```

- The usual rules regarding constexpr for functions apply: if the lambda is used in a runtime context, the corresponding functionality is performed at run time.
- However, using features in a constexpr lambda that are not valid in a compile-time context results in a compile-time error:

```cpp
auto squared4 = [](auto val) constexpr {
  static int calls = 0;  // ERROR: static variable in compile-time context...
  return val * val;
};
```

- For an implicit or explicit constexpr lambda, the function call operator is `constexpr`.
  - since C++17, the generated function call operator is `constexpr` if either the lambda is explicitly defined to be `constexpr` or the lambda is implicitly `constexpr`

```cpp
auto squared = [](auto val) {  // implicitly constexpr since C++17
  return val * val;
};
```

- E.g. converts into the closure type:


```cpp
class CompilerSpecificName {
 public:
  template <typename T>
  // function call operator of the generated closure type is automatically constexpr here.
  constexpr auto operator()(T val) const {
    return val * val;
  }
};
```

- Note the difference below:

```cpp

// `squared1: The lambda itself is declared as constexpr.
// This means that the lambda can be used at compile time for computations.
// However, the object squared1 is not explicitly declared as constexpr.
// The initialization of squared1 will happen at the point of definition, which
// may occur at runtime. This can lead to potential issues if the static
// initialization order matters in your program. The static initialization order
// fiasco refers to situations where the initialization order of static objects
// becomes unpredictable, causing problems if one object relies on another
// during initialization.

auto squared1 = [](auto val) constexpr {  // compile-time lambda calls
  return val * val;
};

// `squared2`: In this case, the lambda itself is not declared as constexpr, but
// the object squared2 is declared as constexpr. This means that the object
// squared2 is initialized when the program starts and its value is known at
// compile time, but the lambda can only be used at run time.
// The constexpr specifier on the object squared2 ensures that it is initialized
// at compile time and can be used in other constexpr contexts. However, since
// the lambda is not declared as constexpr, it cannot be used in compile-time
// contexts, such as template arguments or constant expressions.

constexpr auto squared2 = [](auto val) {  // compile-time initialization
  return val * val;
};
```

- If (only) the lambda is `constexpr` it can be used at compile time, but `squared1` might be initialized at run time, which means that some problems might occur if the static initialization order matters (e.g., causing the static initialization order fiasco).
- If the (closure) object initialized by the lambda is `constexpr`, the object is initialized when the program starts but the lambda might still be a lambda that can only be used at run time (e.g., using `static` variables). Therefore, you might consider declaring as this instead:

```cpp
// both places are now declared as constepxr!
constexpr auto squared = [](auto val) constexpr {
  return val * val;
};
```

### [TODO] 6.1.1


## 6.2. Passing Copies of `this` to Lambdas through capture `*this`

- When using lambdas in non-static member functions, you have no implicit access to the object the member function is called for. That is, inside the lambda, you cannot use members of the object without capturing this in some form

```cpp
class C {
 private:
  std::string name;

 public:
  ... void foo() {
    auto l1 = [] { std::cout << name << '\n'; };        // ERROR
    auto l2 = [] { std::cout << this->name << '\n'; };  // ERROR
    ...
  }
}
```

- In C++11 and C++14, you have to pass this either by value or by reference:

```cpp
class C {
 private:
  std::string name;

 public:
  ... void foo() {
    auto l1 = [this] { std::cout << name << '\n'; };  // OK
    auto l2 = [=] { std::cout << name << '\n'; };     // OK
    auto l3 = [&] { std::cout << name << '\n'; };     // OK
    ...
  }
};
```

- However, the problem here is that even copying this captures the underlying object by reference (as only the pointer was copied).
- This can become a problem if the lifetime of the lambda exceeds the lifetime of the object upon which the member function is invoked.
  - One critical example is when the lambda defines the task of a new thread which should use its own copy of the object to avoid any concurrency or lifetime issues.
  - Another reason might simply be to pass a copy of the object with its current state.
- A workaround has been possible since C++14 but it does not read and work well

```cpp
class C {
 private:
  std::string name;

 public:
  ...
  void foo() {
    auto l1 = [thisCopy = *this] { std::cout << thisCopy.name << '\n'; };
    ...
  }
};
```

- For example, programmers could still accidentally use `this` when also using `=` or `&` to capture other objects, (which brings in the `this`):


```cpp
auto l1 = [&, thisCopy = *this] {
  thisCopy.name = "new name";
  std::cout << name << '\n';  // OOPS: still the old name, as `this` is captured
};
```

- Since C++17, you can **explicitly ask to capture a copy of the current object** by capturing `*this`:

```cpp
class C {
 private:
  std::string name;

 public:
  ...
  void foo() {
    //the capture *this means that a copy of the current object is stored in the
    //closure object defined by the lambda.
    auto l1 = [*this] { std::cout << name << '\n'; };
    ...
  }
};

// You can still combine capturing *this with other captures, as long as there
// is no contradiction for handling this:
auto l2 = [&, *this] { ... };     // OK
auto l3 = [this, *this] { ... };  // ERROR
```

## [6.3 Capturing by const Reference](ch25.other_utility_n_algos.md#2521-capturing-by-const-reference)

- Using a new library `utility`, you can now also capture objects by const reference.


```cpp
std::vector<int> coll{8, 15, 7, 42};
auto printColl = [&coll = std::as_const(coll)] {
  std::cout << "coll: ";
  for (int elem : coll) {
    std::cout << elem << ' ';
  }
  std::cout << '\n';
}
```
