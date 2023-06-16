# [C++17 - The complete guide, Ch 4: Aggregate Extensions](http://www.cppstd17.com/index.html)

- Since C++17, aggregates can have base classes, which means that for such structures, that are derived from other classes/structures, list initialization is allowed:

```cpp
struct Data {
  std::string name;
  double value;
};

struct MoreData : Data {
  bool done;
};

MoreData y{{"test1", 6.778}, false};
//  you can also even skip nested braces:
MoreData y{"test1", 6.778, false};
```


```cpp
struct Data {
  const char* name;
  double value;
};
struct CppData : Data {
  bool critical;
  void print() const { std::cout << '[' << name << ',' << value << "]\n"; }
};
CppData y{{"test1", 6.778}, false};

CppData x1{};          // zero-initialize all elements
CppData x2{{"msg"}};   // same as {{"msg",0.0},false}
CppData x3{{}, true};  // same as {{nullptr,0.0},true}
CppData x4;            // values of fundamental types are unspecified
```

- Note the difference between using empty curly braces and no braces at all:
  - The definition of x1 initializes all members so that the character pointer name is initialized by nullptr, the double value is initialized by 0.0, and the bool flag is initialized by false.
  - The definition of x4 does not initialize any member. All members have an unspecified value.


- You can even derive aggregates from multiple base classes and/or aggregates:

```cpp
template <typename T>
struct D : std::string, std::complex<T> {
  std::string data;
};
D<float> s{{"hello"}, {4.5, 6.7}, "world"};  // OK since C++17
D<float> t{"hello", {4.5, 6.7}, "world"};    // OK since C++17
```

- There are some constraints about when a class can't be "aggregate" ... check out Ch 4.3...