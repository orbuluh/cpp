# [C++17 - The complete guide, Ch 3: Inline variables](http://www.cppstd17.com/index.html)

## 3.1 Motivation for Inline Variables

- In C++, initializing a non-const static member inside the class structure is not allowed

```cpp
class MyClass {
  static std::string msg{"OK"};  // compile-time ERROR
  ...
};
```

- Defining the variable outside the class structure is also an error if this definition is part of a header file included by multiple CPP files:

```cpp
class MyClass {
  static std::string msg;
  ...
};
std::string MyClass::msg{"OK"};  // Link ERROR if included by multiple CPP files
```

- According to the one definition rule (ODR), a variable or entity has to be defined in **exactly one translation unit**—unless the variable or entity is declared to be `inline`.

- Even include guards do not help:

```cpp
#ifndef MYHEADER_HPP
#define MYHEADER_HPP
class MyClass {
  static std::string msg;
  ...
};
std::string MyClass::msg{"OK"};  // Link ERROR if included by multiple CPP files
#endif
```

- The problem is not that the header file might be included multiple times; the problem is that two different CPP files include the header so that both define `MyClass::msg`.
- For the same reason, you get a link error if you define an object of your class in a header file:

```cpp
class MyClass {
  ...
};
MyClass myGlobalObject;  // Link ERROR if included by multiple CPP files
```

### Workarounds

For some cases, there are workarounds:

You can initialize `static const` data members of integral and enumeration types in a class/struct:

```cpp
class MyClass {
  static const bool trace = false; // OK, literal type
  ...
};
```

- However, this is only allowed for literal types, such as fundamental integral, floating-point, or pointer types, or for classes with only constant expressions to initialize non-static data members and no user-defined or virtual destructor.
- In addition, you still need a definition in one translation unit once you need the location of the member (e.g., by binding it to a reference).


You can define an inline function that returns a static local variable:

```cpp
inline std::string& getMsg() {
  static std::string msg{"OK"};
  return msg;
}
```

You can define a static member function that returns the value:

```cpp
class MyClass {
  static std::string& getMsg() {
    static std::string msg{"OK"};

    return msg;
  }
  ...
};
```

You can use variable templates (since C++14):

```cpp
template <typename T = std::string>
T myGlobalMsg{"OK"};
```


You can define a class template for the static member(s):


```cpp
template <typename = void>
class MyClassStatics {
  static std::string msg;
};
template <typename T>
std::string MyClassStatics<T>::msg{"OK"};

// and then even derive from it:

class MyClass : public MyClassStatics<> {
...
};
```

- However, all of these approaches lead to significant overhead, less readability, and/or different ways to use the global variable.
- In addition, the initialization of a global variable might be postponed until its first use, which disables applications in which we want to initialize objects at program start (such as when using an object to monitor the process).

## 3.2 Using Inline Variables

- Now, with inline, you can have a single globally available object by defining it only in a header file, which might be included by multiple CPP files:

```cpp
class MyClass {
inline static std::string msg{"OK"}; // OK since C++17
...
};
inline MyClass myGlobalObj; // OK even if included/defined by multiple CPP files
```

- Formally, the inline used here has the same semantics as a function declared inline:
  - It can be defined in multiple translation units, provided all definitions are identical.
  - It must be defined in every translation unit in which it is used.

- Both are given by including the definition from the same header file. The resulting behavior of the program is as if there were exactly one variable.
- You can even apply this to define atomic types in header files only:

`inline std::atomic<bool> ready{false};`

- As usual for `std::atomic`, you always have to initialize the values when you define them.
  - Note that you still have to ensure that types are complete before you can initialize them. For example, if a struct or class has a static member of its own type, the member can only be defined inline after the type declaration:

```cpp
struct MyType {
  int value;
  MyType(int i) : value{i} {}
  // one static object to hold the maximum value of this type:
  static MyType max;  // can only be declared here
  ...
};
inline MyType MyType::max{0};
```



## 3.3 `constexpr` Now Implies `inline` For Static Members

- For static data members, `constexpr` implies `inline` now, such that since C++17, the following declaration defines the static data member `n`:

```cpp
struct D {
  static constexpr int n = 5; // C++11/C++14: de"claration"
                              // since C++17: de"finition"
};
```

- That is, it is the same as:

```cpp
struct D {
  inline static constexpr int n = 5;
};
```

- Note that before C++17, you could often have the declaration only without a corresponding definition.
- Consider the following declaration:

```cpp
struct D {
  static constexpr int n = 5;
};
```

- This was enough if no definition of D::n was needed, which was the case if D::n was only passed by value:

```cpp
std::cout << D::n; // OK (ostream::operator<<(int) gets D::n by value)
```

- If `D::n` was passed by reference to a non-inlined function and/or the call was not optimized away, this was invalid. For example:

```cpp
int twice(const int& i);
std::cout << twice(D::n); // was often an ERROR
```

- This code violated the one definition rule (ODR). When built with an optimizing compiler, it might have worked as expected or might have given a link error due to the missing definition.
- When built without any optimizations, it will almost certainly be rejected due to the missing definition of `D::n`
- Creating a pointer to the static member does even more likely result in a link error due to the missing definition (but it might still work with some compiler modes):

```cpp
const int* p = &D::n; // was usually an ERROR
```

- As a consequence, before C++17, you had to define D::n in exactly one translation unit:

```cpp
constexpr int D::n; // C++11/C++14: definition
                    // since C++17: redundant declaration (deprecated)
```

- Now, when built with C++17, the declaration inside the class is a definition by itself, so all examples above are now valid without the former definition.
- The former definition is still valid but a deprecated redundant declaration.

## 3.4 Inline Variables and `thread_local`

By using `thread_local` you can also make an inline variable unique for each thread:

```cpp
struct ThreadData {
inline static thread_local std::string name; // unique name per thread
...
};
inline thread_local std::vector<std::string> cache; // one cache per thread
```

Headers looks like:

```cpp
// inlinethreadlocal.hpp
#include <iostream>
#include <string>
struct MyData {
  inline static std::string gName = "global";            // unique in program
  inline static thread_local std::string tName = "tls";  // unique per thread for each object
  std::string lName = "local";
  ...
  void print(const std::string& msg) const {
    std::cout << msg << '\n';
    std::cout << "- gName: " << gName << '\n';
    std::cout << "- tName: " << tName << '\n';
    std::cout << "- lName: " << lName << '\n';
  }
};
inline thread_local MyData myThreadData;  // one object per thread
```

First cpp file including the header

```cpp
#include "inlinethreadlocal.hpp"
void foo() {
  myThreadData.print("foo() begin:");
  myThreadData.gName = "thread2 name";
  myThreadData.tName = "thread2 name";
  myThreadData.lName = "thread2 name";
  myThreadData.print("foo() end:");
}
```

Main also includes the header

```cpp
#include <thread>

#include "inlinethreadlocal.hpp"
void foo();
int main() {
  myThreadData.print("main() begin:");
  myThreadData.gName = "thread1 name";
  myThreadData.tName = "thread1 name";
  myThreadData.lName = "thread1 name";
  myThreadData.print("main() later:");
  std::thread t(foo);
  t.join();
  myThreadData.print("main() end:");
}
```

The output result:

- The gName is shared by all object, hence at the end it's "thread 2 name..."
- tName are per thread, hence at hte end, it's still "thread 1 name..."
- myThreadData is one object per thread, hence lName belongs to different object in different thread. At the end, it's still "thread 1 name..."


```bash
main() begin:
- gName: global
- tName: tls
- lName: local
main() later:
- gName: thread1 name
- tName: thread1 name
- lName: thread1 name
foo() begin:
- gName: thread1 name
- tName: tls
- lName: local
foo() end:
- gName: thread2 name
- tName: thread2 name
- lName: thread2 name
main() end:
- gName: thread2 name
- tName: thread1 name
- lName: thread1 name
```