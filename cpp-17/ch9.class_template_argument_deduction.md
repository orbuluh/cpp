# [C++17 - The complete guide, Ch 9:　Class Template Argument Deduction](http://www.cppstd17.com/index.html)

## Class Template Argument Deduction

Before C++17, you always had to explicitly specify all template arguments for class templates. For example, you cannot omit the double here:

```cpp
std::complex<double> c{5.1,3.3};
```

or omit the need to specify `std::mutex` here a second time:

```c++
std::mutex mx;
std::lock_guard<std::mutex> lg(mx);
```

- Since C++17, the constraint of always having to specify the template arguments explicitly has been relaxed. By using class template argument deduction (CTAD), you can omit explicit definition of the template arguments if the constructor is able to deduce all template parameters.
- You can now declare:

```cpp
std::complex c{5.1,3.3}; // OK: std::complex<double> deduced
```

-  You can now implement:

```cpp
std::mutex mx;
std::lock_guard lg{mx}; // OK: std::lock_guard<std_mutex> deduced
```

- You can even let containers deduce element types:

```cpp
std::vector v1 {1, 2, 3} // OK: std::vector<int> deduced
std::vector v2 {"hello", "world"}; // OK: std::vector<const char*> deduced
```

## 9.1 Use of Class Template Argument Deduction

Class template argument deduction can be used whenever the arguments passed to a constructor can be used to deduce the class template parameters.

The deduction supports all methods of initialization (provided the initialization itself is valid):

```cpp
std::complex c1{1.1, 2.2}; // deduces std::complex<double>
std::complex c2(2.2, 3.3); // deduces std::complex<double>
std::complex c3 = 3.3; // deduces std::complex<double>
std::complex c4 = {4.4}; // deduces std::complex<double>
```

c3 and c4 can be initialized because you can initialize a `std::complex<>`` by passing only one argument, which is enough to deduce the template parameter T, which is then used for both the real and the imaginary part:

```cpp
namespace std {
  template<typename T>
  class complex {
  constexpr complex(const T& re = T(), const T& im = T());
  ...
  }
};

```

With a declaration such as

```cpp
std::complex c1{1.1, 2.2};
```

- the compiler finds the constructor `constexpr complex(const T& re = T(), const T& im = T());` as a possible function to call.
- For both arguments, T is double, Therefore, the compiler deduces T to be double and compiles the corresponding code for:
`complex<double>::complex(const double& re = double(), const double& im = double());`

- Note that the template parameter has to be unambiguously deducible. Thus, the following initialization does not work:

```cpp
std::complex c5{5,3.3}; // ERROR: attempts to int and double as T
```

- As usual for templates, there are **no type conversions used to deduce template parameters.**


- Class template argument deduction for variadic templates is also supported. For example, for a `std::tuple<>` defined as:

```cpp
namespace std {
  template<typename... Types>
  class tuple {
  public:
    constexpr tuple(const Types&...);
    ...
  };
};
```

- the declaration: `std::tuple t{42, 'x', nullptr};` deduces the type of t as `std::tuple<int, char, std::nullptr_t>`.

- You can also deduce non-type template parameters. For example, we can deduce template parameters for both the element type and the size from a passed initial array as follows:

```cpp
template<typename T, int SZ>
class MyClass {
public:
  MyClass (T(&)[SZ]) {
  ...
  }
};
// MyClass mc("hello"); // deduces T as const char and SZ as six
```

- Here we deduce 6 as SZ because the template parameter passed is a string literal with 6 characters.
- You can even deduce the type of lambdas used as base classes for overloading or deduce the type of `auto` template parameters.


### 9.1.1 Copying by Default

- If class template argument deduction could be interpreted as initializing a copy, this interpretation is preferred.
  - For example, after initializing a `std::vector`` with one element:

```cpp
std::vector v1{42}; // vector<int> with one element
```

using that vector as an initializer for another vector is interpreted as creating a copy:

```cpp
std::vector v2{v1}; // v2 also is a vector<int>
```

instead of assuming that a vector gets initialized having elements being vectors (vector<vector<int>>).

Again, this applies to all valid forms of initialization:

```cpp
std::vector v2{v1}; // v3 also is a vector<int>
std::vector v3(v1); // v3 also is a vector<int>
std::vector v4 = {v1}; // v4 also is a vector<int>
auto v5 = std::vector{v1}; // v5 also is a vector<int>
```

- Note that this means that there is an exception to the rule that initializations with curly braces always use the passed arguments as elements.
- If you pass an initializer list with a single vector argument, you create a copy of the vector.
- However, passing an initializer list with more than one element always deduces the type of the elements as template parameter (because this cannot be interpreted as creating a copy):

```cpp
std::vector vv{v1, v2}; // vv is vector<vector<int>>
```

This raises the question of what happens with class template argument deduction when passing variadic templates:

```cpp
template<typename... Args>
auto make_vector(const Args&... elems) {
  return std::vector{elems...};
}
std::vector<int> v{1, 2, 3};

auto x1 = make_vector(v, v); // vector<vector<int>>
auto x2 = make_vector(v); // vector<int> or vector<vector<int>> ?
```

- Currently, different compilers handle this differently and the issue is under discussion.


### 9.1.2 Deducing the Type of Lambdas

With class template argument deduction, we can instantiate class templates with the type of a lambda (to be precise: the closure type of a lambda).

- For example, we could provide a generic class that wraps and counts calls of an arbitrary callback:

```cpp
// tmpl/classarglambda.hpp
✞ ☎
#include <utility>  // for std::forward()
template <typename CB>
class CountCalls {
 private:
  CB callback;     // callback to call
  long calls = 0;  // counter for calls
 public:
  CountCalls(CB cb) : callback(cb) {}
  template <typename... Args>
  decltype(auto) operator()(Args&&... args) {
    ++calls;
    return callback(std::forward<Args>(args)...);
  }
  long count() const { return calls; }
};
```

 ✆
- Here, the constructor, taking the callback to wrap, enables the deduction of its type as the template parameter `CB`.
- For example, we can initialize an object passing a lambda as an argument:

```cpp
CountCalls sc{[](auto x, auto y) {
return x > y;
}};
```

- which means that the type of the sorting criterion sc is deduced as CountCalls<TypeOfTheLambda>.
- This way, we can count the number of calls for a passed sorting criterion:

```cpp
std::sort(v.begin(), v.end(), // range
          std::ref(sc)); // sorting criterion ... sort takes lambda by value, so we have to wrap with ref
std::cout << "sorted with " << sc.count() << " calls\n";
```

- Here, the wrapped lambda is used as a sorting criterion. It has to be passed by reference, because otherwise, `std::sort()` uses only its own copy of the passed counter, as `std::sort()` takes the sorting criterion by value.
- However, we can pass a wrapped lambda to std::for_each() because this algorithm (in the nonparallel version) returns its own copy of the passed callback to be able to use its resulting state:

```cpp
auto fo = std::for_each(v.begin(), v.end(),
                        CountCalls{[](auto i) {
                                   std::cout << "elem: " << i << '\n';
}});
std::cout << "output with " << fo.count() << " calls\n";
```

- The output will be something like the following (the sorted count might vary because the implementation of sort() might vary):

```cpp
sorted with 39 calls
elem: 19
elem: 17
elem: 13
elem: 11
elem: 9
elem: 7
elem: 5
elem: 3
elem: 2
output with 9 calls
```


- If the counter for the calls is atomic, you could also use parallel algorithms:
std::sort(std::execution::par,
          v.begin(), v.end(),
          std::ref(sc));

### 9.1.3 No Partial Class Template Argument Deduction

- Note that, unlike function templates, **class template arguments may not be partially deduced by explicitly specifying only some of the template arguments.**
- Not even passing an empty list of argument with <> is allowed. For example:

```cpp
template <typename T1, typename T2, typename T3 = T2> // Note that the third template parameter has a default value.
class C {
 public:
  C(T1 x = {}, T2 y = {}, T3 z = {}){...}...
};
// all deduced:
C c1(22, 44.3, "hi");  // OK: T1 is int, T2 is double, T3 is const char*
C c2(22, 44.3);        // OK: T1 is int, T2 and T3 are double
C c3("hi", "guy");     // OK: T1, T2, and T3 are const char*
// only some deduced:
C<string> c4("hi", "my");  // ERROR: only T1 explicitly defined
C<> c5(22, 44.3);          // ERROR: neither T1 not T2 explicitly defined
C<> c6(22, 44.3, 42);      // ERROR: neither T1 nor T2 explicitly defined
// all specified:
C<string, string, int> c7;            // OK: T1,T2 are string, T3 is int
C<int, string> c8(52, "my");          // OK: T1 is int,T2 and T3 are strings
C<string, string> c9("a", "b", "c");  // OK: T1,T2,T3 are strings
```

-  For this reason, you do not have to explicitly specify the last type if the second type is specified.
- If you are wondering why partial specialization is not supported, here is the example that caused this decision:


```cpp
std::tuple<int> t(42, 43); // still ERROR
```

- std::tuple is a variadic template, so you could specify an arbitrary number of arguments.
- In this case, therefore, it is not clear whether specifying only one type is an error or whether this is intentional. It looks questionable at least.
- Unfortunately, the inability to partially specialize means that a common unfortunate coding requirement is not solved. We still cannot easily use a lambda to specify the sorting criterion of an associative container or the hash function of an unordered container:

```cpp
std::set<Cust> coll([](const Cust& x, const Cust& y) { // still ERROR
  return x.getName() > y.getName();
});
```

- We still have to specify the type of the lambda. For example:
```cpp
auto sortcrit = [](const Cust& x, const Cust& y) {
  return x.getName() > y.getName();
};
std::set<Cust, decltype(sortcrit)> coll(sortcrit); // OK
```

- Specifying only the type does not work because then the container tries to create a lambda of the given type, which is not allowed in C++17, because **the default constructor can be called only by the compiler**. With C++20, this will be possible, provided the lambda does not capture anything.


### 9.1.4 Class Template Argument Deduction Instead of Convenience Functions

- In principle, by using class template argument deduction, we can get rid of several convenience function templates that existed only to allow the deduction of class template parameters from the passed call arguments.

- The obvious example is `std::make_pair()`, which avoided the need to specify the type of the passed arguments. For example, 

```cpp
std::vector<int> v;
auto p = std::make_pair(v.begin(), v.end());
```

instead of writing:

```cpp
std::pair<typename std::vector<int>::iterator,
typename std::vector<int>::iterator> p(v.begin(), v.end());
```

- Here, `std::make_pair()` is no longer needed, as we can now simply declare:

```cpp
std::pair p(v.begin(), v.end());
```

or

```cpp
std::pair p{v.begin(), v.end()};
```


- However, `std::make_pair()` is also a good example to demonstrate that sometimes the convenience functions do more than just deduce template parameters.
- In fact, `std::make_pair()` **decays the passed argument** (passing them by value in C++03 and using traits since C++11). This has the effect that the type of passed string literals (arrays of characters) are deduced as `const char*`:

```cpp
auto q = std::make_pair("hi", "world"); // deduces pair of pointers
```

- In this case, q has type `std::pair<const char*, const char*>`.
-  Using class template argument deduction makes things more complicated. Look at the relevant part of a simple class declaration like `std::pair`:

```cpp
template <typename T1, typename T2>
struct Pair1 {
  T1 first;
  T2 second;
  Pair1(const T1& x, const T2& y) : first{x}, second{y} {}
};
```


- The elements are passed by reference and according to language rules, **when passing arguments of a template type by reference, the parameter type does not decay**, which is the term for the mechanism to convert a raw array type to the corresponding raw pointer type.
- Therefore, when calling:

```cpp
Pair1 p1{"hi", "world"}; // deduces pair of arrays of different size, but...
```

- T1 is deduced as char[3] and T2 is deduced as char[6]. In principle, such a deduction is valid.
- However, we use T1 and T2 to declare the members first and second. As a consequence, they are declared as

```cpp
char first[3];
char second[6];
```

- and **initializing an array from an lvalue of an array is not allowed.** It is like trying to compile:

```cpp
const char x[3] = "hi";
const char y[6] = "world";
char first[3] {x}; // ERROR
char second[6] {y}; // ERROR
```


Note that we would not have this problem when declaring the parameter to be passed by value:

```cpp
template <typename T1, typename T2>
struct Pair2 {
  T1 first;
  T2 second;
  Pair2(T1 x, T2 y) : first{x}, second{y} {}
};
```


- If we would create an object of this type as follows:

```cpp
Pair2 p2{"hi", "world"}; // deduces pair of pointers
```

- T1 and T2 would both be deduced as `const char*`. However, because the constructors of class `std::pair<>` take the arguments by reference, the following initialization should normally not compile:

```cpp
std::pair p{"hi", "world"}; // seems to deduce pair of arrays of different size, but...
```

- However, it does compile because class `std::pair<>` has deduction guides, which we discuss in the next section.



## 9.2 Deduction Guides


You can define specific deduction guides to provide additional class template argument deductions or fix existing deductions defined by constructors.

- For example, you can define that whenever the types of a Pair3 are deduced, the type deduction should operate as if the types had been passed by value:

```cpp
template <typename T1, typename T2>
struct Pair3 {
  T1 first;
  T2 second;
  Pair3(const T1& x, const T2& y) : first{x}, second{y} {}
};
// deduction guide for the constructor:
template <typename T1, typename T2>
Pair3(T1, T2) -> Pair3<T1, T2>;
```

- Here, on the left side of the ->, we declare what we want to deduce. In this case, it is the creation of a Pair3 from two objects of arbitrary types T1 and T2 passed by value.
- On the right side of the ->, we define the resulting deduction.
- In this example, `Pair3` is instantiated with the two types `T1` and `T2`.
- You might argue that this is what the constructor does already. However, the constructor takes the arguments by reference, which is not the same. In general, even outside templates, arguments passed by value decay, while arguments passed by reference do not decay.
- Decay means that raw arrays convert to pointers and that top-level qualifiers, such as `const`, and references are ignored.
- Without the deduction guide, for example, when declaring the following:

```cpp
Pair3 p3{"hi", "world"};
```

- the type of parameter x is `const char(&)[3]` and therefore `T1` is `char[3]`, and the type of parameter `y` is
`const char(&)[6]` and therefore `T2` is `char[6]`.
- Due to the deduction guide, the template parameters decay, which means that passed arrays or string literals decay to the corresponding pointer types.
- Now, when declaring the following:

```cpp
Pair3 p3{"hi", "world"};
```

- the deduction guide is used that takes the parameters by value. Therefore, both types decay to const char*, which is used a deduced template parameters.
- The declaration has the same effect as if we had declared:

```cpp
Pair3<const char*, const char*> p3{"hi", "world"};
```


- Note that the constructor still takes the arguments by reference. The **deduction guide is relevant only for the deduction of the template types**; it is irrelevant for the actual constructor call after the types `T1` and `T2` are deduced.


### 9.2.1 Using Deduction Guides to Force Decay

As the previous example demonstrates, in general, a very useful application of these overloading rules is to ensure that a template parameter `T` decays while it is deduced. Consider a typical class template:

```cpp
template<typename T>
struct C {
C(const T&) {
}
...
};
```

- Here, if we pass a string literal "hello", we pass type const char(&)[6] so that `T` is deduced as `const char[6]`:

```cpp
C x{"hello"}; // T deduced as char[6]
```

- The reason for this is that template parameter deduction does not decay to the corresponding pointer type when arguments are passed by reference.

- With a simple deduction guide

```cpp
template<typename T> C(T) -> C<T>;
```

we can fix this problem:

```cpp
C x{"hello"}; // T deduced as const char*
```

- The deduction guide takes its argument by value and therefore its type decays, which means that "hello" deduces `T` to be of type `const char*`.
- For this reason, a corresponding deduction guide sounds very reasonable for any class template that has a constructor that takes an object of its template parameter by reference. The C++ standard library provides corresponding deduction guides for pairs and tuples.


### 9.2.2 Non-Template Deduction Guides

- Deduction guides do not have to be templates and do not have to apply to constructors.

- For example, implementing the following structure and deduction guide is valid:

```cpp
template<typename T>
struct S {
  T val;
};
S(const char*) -> S<std::string>; // map S<> for string literals to S<std::string>
```

- Here, we have a deduction guide without a constructor.
- The deduction guide is used to deduce parameter T, but then the structure is used as if the template parameter had been specified explicitly.
- For this reason, the following declarations are possible and deduce `std::string` as class template parameter `T`:

```cpp
S s1{"hello"}; // OK, same as: S<std::string> s1f"hello"g;
S s2 = {"hello"}; // OK, same as: S<std::string> s2 = f"hello"g;
S s3 = S{"hello"}; // OK, both S deduced to be S<std::string>
```


- All initializations are valid because the passed string literals implicitly convert to `std::string`
- Note that **aggregates need list initialization**. The following deductions work, but the initializations are not allowed because no braces are used:

```cpp
S s4 = "hello"; // ERROR: can’t initialize aggregates without braces
S s5("hello"); // ERROR: can’t initialize aggregates without braces
```



### 9.2.3 Deduction Guides versus Constructors

- Deduction guides compete with the constructors of a class.
- Class template argument deduction uses the constructor/guide that has the highest priority according to overload resolution. If a constructor and a deduction guide match equally well, the deduction guide is preferred.
- Consider the following definition:

```cpp
template<typename T>
  struct C1 {
  C1(const T&) {
  }
};
C1(int) -> C1<long>;
```

- When passing an `int`, the deduction guide is used because it is preferred by overload resolution.2 Therefore, `T` is deduced as `long`:


```cpp
C1 x1{42}; // T deduced as long
```


- However, if we pass a char, the constructor is a better match (because no type conversion is necessary), which means that we deduce `T` to be `char`:


```cpp
C1 x3{'x'}; // T deduced as char
```


- In overload resolution, there is no preference between call-by-value and call-by-reference.
- However, deduction guides are preferred for equally good matches. Therefore, **it is usually fine to let the deduction guide take the argument by value (which also has the advantage of decaying).**



### 9.2.4 Explicit Deduction Guides

- A deduction guide can be declared as `explicit`, indicating that it should not be used implicitly for certain cases.
- It is then ignored for cases where the `explicit` would disable initializations or conversions.
- For example, given:

```cpp
template<typename T>
struct S {
  T val;
};

// The deduction guide specifies that when the constructor is called with a
// const char* argument, it should deduce T as std::string.
explicit S(const char*) -> S<std::string>;
```

- a copy initialization (using the =) ignores the deduction guide. In this case, it means that the initialization becomes invalid:

```cpp
// copy ctor is called and error
// This is because the explicit deduction guide is not automatically applied
// for copy initialization.
S s1 = {"hello"}; // ERROR (deduction guide ignored and otherwise invalid)
```

- However, direct initialization or an explicit deduction on the right-hand side is still possible:

```cpp
S s2{"hello"}; // OK, same as: S<std::string> s2{"hello"};
S s3 = S{"hello"}; // OK, using the deduction guide explicitly then copy assign
S s4 = {S{"hello"}}; // OK, direct initialization and the explicit deduction guide
```

- As another example, we could do the following:

```cpp
template <typename T>
struct Ptr {
  Ptr(T) { std::cout << "Ptr(T)\n"; }
  template <typename U>
  Ptr(U) {
    std::cout << "Ptr(U)\n";
  }
};

template <typename T>
explicit Ptr(T) -> Ptr<T*>;
```

which would have the following effect:


```cpp
Ptr p1{42}; // deduces Ptr<int*> due to deduction guide
Ptr p2 = 42; // deduces Ptr<int> due to constructor
int i = 42;
Ptr p3{&i}; // deduces Ptr<int**> due to deduction guide
Ptr p4 = &i; // deduces Ptr<int*> due to constructor
```


### 9.2.5 Deduction Guides for Aggregates

- Deduction guides can be used in generic aggregates to enable class template argument deduction.
- For example, for:

```cpp
template<typename T>
struct A {
  T val;
};
```

- any attempt of class template argument deduction without a deduction guide is an error:


```cpp
A i1{42}; // ERROR
A s1("hi"); // ERROR
A s2{"hi"}; // ERROR
A s3 = "hi"; // ERROR
A s4 = {"hi"}; // ERROR
```


You have to pass the argument for type `T` explicitly:

```cpp
A<int> i2{42};
A<std::string> s5 = {"hi"};
```


- However, after a deduction guide such as:


```cpp
A(const char*) -> A<std::string>;
```

- you can initialize the aggregate as follows:

```cpp
A s2{"hi"}; // OK
A s4 = {"hi"}; // OK
```


- Note that (as usual for aggregate initialization) you still need curly braces.
- Otherwise, type `T` is successfully deduced but the initialization is an error:

```cpp
A s1("hi"); // ERROR: T is string, but no aggregate initialization
A s3 = "hi"; // ERROR: T is string, but no aggregate initialization
```


- The deduction guides for `std::array` are further examples of deduction guides for aggregates.



### 9.2.6 Standard Deduction Guides

- The C++ standard library introduces a couple of deduction guides with C++17.



**Deduction Guides for Pairs and Tuples**

---

- As introduced in the motivation for deduction guides, `std::pair` needs deduction guides to ensure that class template argument deduction uses the decayed type of the passed argument:


```cpp
namespace std {
template<typename T1, typename T2>
struct pair {
...
constexpr pair(const T1& x, const T2& y); // take argument by-reference
...
};

template<typename T1, typename T2>
pair(T1, T2) -> pair<T1, T2>; // deduce argument types by-value
}
```

- As a consequence, the declaration

```cpp
std::pair p{"hi", "world"}; // takes const char[3] and const char[6]
```

- is equivalent to:

```cpp
std::pair<const char*, const char*> p{"hi", "world"};
```

- For the variadic class template `std::tuple`, the same approach is used:

```cpp
namespace std {
template<typename... Types>
class tuple {
public:
  constexpr tuple(const Types&...); // take arguments by-reference
  template<typename... UTypes> constexpr tuple(UTypes&&...);
  //...
};
template<typename... Types>
tuple(Types...) -> tuple<Types...>; // deduce argument types by-value
  //...
};
```

As a consequence, the declaration:

```cpp
std::tuple t{42, "hello", nullptr};
deduces the type of t as std::tuple<int, const char*, std::nullptr_t>.
```

**Deduction From Iterators**

---

- To be able to deduce the type of the elements from iterators that define a range for initialization, containers have a deduction guide such as the following for `std::vector<>`:

```cpp

// let std::vector<> deduce element type from initializing iterators:
namespace std {
template<typename Iterator>
vector(Iterator, Iterator) -> vector<typename iterator_traits<Iterator>::value_type>;
}
```

- This allows, for example:

```cpp
std::set<float> s;
std::vector v1(s.begin(), s.end()); // OK, deduces std::vector<float>
```


- Note that the use of initialization with parentheses is important here. If you use curly braces:

```cpp
std::vector v2{s.begin(), s.end()}; // BEWARE: doesn’t deduce std::vector<float>
```

- the two arguments are taken as elements of an initializer list (which has a higher priority according to the overload resolution rules). That is, it is equivalent to:

```cpp
std::vector<std::set<float>::iterator> v2{s.begin(), s.end()};
```

- meaning that we initialize a vector of two elements, the first referring to the first element and the second representing the position behind the last element.
- On the other hand, consider:

```cpp
std::vector v3{"hi", "world"}; // OK, deduces std::vector<const char*>
std::vector v4("hi", "world"); // OOPS: fatal runtime error, two iterators one pointing to `h` and one pointing to `w`
```

- While the declaration of v3 also initializes the vector with two elements (both being string literals), the second causes a fatal runtime error, which hopefully causes a core dump.
- The problem is that string literals convert to character pointers, which are valid iterators.
- Thus, we pass two iterators that do not point to the same object.
- In other words: we pass an invalid range. We deduce a `std::vector<const char>`, but depending on the location of the two literals in memory, we get a bad_alloc exception, or we get a core dump because there is no distance at all, or we get a range of some undefined characters stored in between.
- To summarize, **using curly braces is always the best way to initialize the elements of a vector.**
  - The only exception is when a single vector is passed (where the copy constructor is preferred).
  - When passing something else, it is better to use parentheses.

- In any case, for a type with complicated constructors such as std::vector<> and other STL containers, it is highly **recommended not to use class template argument deduction and instead, to specify the element type(s) explicitly**.

**std::array<> Deduction**

---

- A more interesting example is provided by class `std::array<>`. To be able to deduce both the element type and the number of elements:


```cpp
std::array a{42, 45, 77}; // OK, deduces std::array<int,3>
```


- the following deduction guide is defined (indirectly)


```cpp

// let std::array<> deduce its number of elements (must have same type):
namespace std {
template<typename T, typename... U>
array(T, U...) -> array<enable_if_t<(is_same_v<T,U> && ...), T>,
(1 + sizeof...(U))>;
}

```

- The deduction guide uses the fold expression

```cpp
(is_same_v<T,U> && ...)
```

- to ensure that the types of all passed arguments are the same.4 Therefore, the following is not possible:

```cpp
std::array a{42, 45, 77.7}; // ERROR: types differ
```


- Note that an initialization with class template argument deduction works even in compile-time contexts:


```cpp
constexpr std::array arr{0, 8, 15}; // OK, deduces std::array<int,3>
```


**(Unordered) Map Deduction**

---

- The complexity involved in getting deduction guides that behave correctly can be demonstrated by the attempts to define deduction guides for containers that have key/value pairs (map, multimap, unordered_map, unordered_multimap).
  - The elements of these containers have type `std::pair<const keytype, valuetype>`.
  - The `const` is necessary because the location of an element depends on the value of the key, which means that the **ability to modify the key could create inconsistencies inside the container**.
  - For this reason, the first approach in the C++17 standard for a `std::map`:

```cpp
namespace std {
template <typename Key, typename T,
          typename Compare = less<Key>,
          typename Allocator = allocator<pair<const Key, T>>>
class map {
  ...
};
}  // namespace std
```

- was, for the following constructor:

```cpp
map(initializer_list<pair<const Key, T>>,
    const Compare& = Compare(),
    const Allocator& = Allocator());
```

- to define the following deduction guide:


```cpp
namespace std {
template <typename Key, typename T,
          typename Compare = less<Key>,
          typename Allocator = allocator<pair<const Key, T>>>
map(initializer_list<pair<const Key, T>>, Compare = Compare(),
    Allocator = Allocator()) -> map<Key, T, Compare, Allocator>;
}
```

- All arguments are passed by value, therefore this deduction guide enabled the type of a passed comparator or allocator to decay as discussed.
- However, we naively used the same argument types, which meant that the initializer list took a `const` key type.
- As a consequence, the following did not work:


```cpp
std::pair elem1{1,2};
std::pair elem2{3,4};
...
std::map m1{elem1, elem2}; // ERROR with original C++17 guides
```

- Because elem1 and elem2 are deduced as std::pair<int,int>, the deduction guide requiring a `const` type as first type in the pair did not match.
- Therefore, you still had to write the following:

```cpp
std::map<int,int> m1{elem1, elem2}; // OK
```

- As a consequence, in the deduction guide, the const had to be removed:


```cpp
namespace std {
template <typename Key, typename T,
          typename Compare = less<Key>,
          typename Allocator = allocator<pair<const Key, T>>>
map(initializer_list<pair<Key, T>>,  // <-- difference, not const Key but Key
    Compare = Compare(),
    Allocator = Allocator()) -> map<Key, T, Compare, Allocator>;
}
```

- However, to still support the decay of the comparator and allocator, we also have to overload the deduction guide for a pair with const key type.
- Otherwise, the constructor would be used for class template argument deduction when passing a pair with a const key type, meaning that the deduction would differ slightly between passing pairs with const and non-const keys.

**No Deduction Guides for Smart Pointers**

---

- Note that some places in the C++ standard library do not have deduction guides even though you might expect them to be present.
- You might expect to have deduction guides for `shared_ptr` and `unique_ptr`, so that instead of:

```cpp
std::shared_ptr<int> sp{new int(7)};
```

you could just write:

```cpp
std::shared_ptr sp{new int(7)}; // not supported
```

- This does not work automatically because the corresponding constructor is a template, which means that no implicit deduction guide applies:


```cpp
namespace std {
template <typename T>
class shared_ptr {
 public:
  ...
  template <typename Y>
  explicit shared_ptr(Y* p);
  ...
};
}  // namespace std
```

- Y is a different template parameter to `T`, meaning that deducing `Y` from the constructor does not mean that we can deduce type `T`.
- This is a feature to be able to call something like:

```cpp
std::shared_ptr<Base> sp{new Derived( ... )};
```

- The corresponding deduction guide would be simple to provide:

```cpp

namespace std{
template<typename Y> shared_ptr(Y*) -> shared_ptr<Y>;
}
```

- However, this would also mean that this guide is taken when allocating arrays:


```cpp
std::shared_ptr sp{new int[10]}; // OOPS: would deduce shared_ptr<int>
```

- As so often in C++, we run into the nasty C problem that the type of a pointer to one object and an array of objects have or decay to the same type.
- This problem seems dangerous and therefore, the C++ standards committee has decided not to support it.
- For single objects, you still have to call:

```cpp
std::shared_ptr<int> sp1{new int}; // OK
auto sp2 = std::make_shared<int>(); // OK
```

- and for arrays:


```cpp
std::shared_ptr<std::string> p(new std::string[10],
                               [](std::string* p) { delete[] p; });
```

- or, using the new feature to instantiate shared pointers for raw arrays, just:


```cpp
std::shared_ptr<std::string[]> p{new std::string[10]};
```
