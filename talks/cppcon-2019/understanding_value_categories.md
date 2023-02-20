# Back to Basics: Understanding Value Categories - Ben Saks

## Introduction

- :bulb: Value categories aren't really language features. They're semantic properties of expressions
- Understanding them provides valuable insights into:
  - built-in and user-defined operators
  - reference types
  - otherwise-cryptic compiler error messages

## Lvalues and Rvalues have evolved

- In early C, there were 2 value categories, lvalues and rvalues.
- The associated concepts were fairly simple. But this has got complicated
  - Early C++ added classes, const, and references.
  - Modern C++ added rvalue references.
- C++ needed more value catagories to specify the associated behaviors.
- This talk explains value categories from this historical perspective.

## Lvalues and Rvalues

- The name "lvalue" comes from the assignment expression: `E1 = E2`, in which `left` oeprand `E1` must be an lvalue expression.
- :bulb: An lvalue is an expression referring to an object
- :bulb: An object is a region of storage

```cpp
int n; // a definition for an integer object named n
n = 1; // an assignment expression
```

- `n` itself is an expression, `1` itself is an expression, `n = 1` is also an expression.
  - n is a sub-expression referring to an `int` object, it's an lvalue.
  - 1 is a sub-expression not referring to an object, it's an rvalue.
- :bulb: An rvalue is simply an expression that's not an lvalue.

- A more complicated example:

```cpp
x[i + 1] = abs(p->value);
```

- `x[i + 1]` is an expression
- `abs(p->value)` is an expression
- For the assignment to be valid:
  - the left operand must be an lvalue. E.g. it must refer to an object.
  - the right operand can be either lvalue or rvalue. It can be any expression.

## Why language designer makes this distinction?

(One answer)

- So tha compilers can assume that rvalues don't necessarily occupy storage.
- This offers considerable freedom in code generation.
- Going back to the `n = 1` assignment example ...

## Data storage for rvalues

- A compiler might place `1` as named data storage, as if `1` were an lvalue.
- In an assembly language, this might look something like:

```asm
one:            ; a label for the following location
    .word 1     ; allocate storage holding the value 1
```

- Then the compiler would copy from that named storage into n

```asm
mov n, one      ; copy the value at one to location n
```

- On the other hand, some machines provide instructions with an immediate operand:
  - A source operand value can be part of an instruction
- In assembly, this might look like:

```asm
mov n, #1       ; copy the value at one to location n
```

- In this case, the value `1` never appears aas an object in data storage.
- Rather, it's part of an instruction in the code space.

## "Must be an Lvalue" means "Can't be an RValue"

- Supposed you write:

```cpp
1 = n;
```

- This tries to change the value of the integer literal `1`
- Of course, C++ rejects it as an error, but why, exactly?
  - It's not because of type, n and 1 are both int
- Reason is, an assignment assigns a value to an object
  - Its left operand must be an lvalue
  - But 1 is not an lvalue, it's an rvalue. It doesn't have any defined place in memory.

## Recap

- Every expression in C++ is either an lvalue or an rvalue
- In general:
  - An lvalue is an expression that refers to an object
  - An rvalue is simply any expression that isn't an lvalue.
- Caveat:
  - This is true for non-class types, but not true for class type.

## Literal

- Most literals are rvlues, including:
  - numeric literals, such as `3` and `3.14159`
  - character literals, such as `'a'`
- They don't necessarily occupy data storage.
- However, character string literals, such as `"abc"` are lvalues; they occupy data storage.

## Lvalues used as rvalues

- An lvalue can appear on either side of an assignment, as in:

```cpp
int m, n;
m = n;
```

- You can assign the value in `n` to the object designated by `m`
- This assignment uses the lvalue expression `n` as an rvalue
- Officially, C++ performs an **lvalue-to-rvalue conversion**

## Operands of other operators

- The concepts of lvalue and rvalue apply in all expressions, not just assignment
- For example, both operands of the binary `+` operator must be expressions.
  - And those expressions must have suitable types.
- But each operand can be either an lvalue or rvalue.

```cpp
int x;

x + 2; // lvalue + rvalue
2 + x; // rvalue + lvalue
```

## What about the result?

- For built-in binary (non-assignment) operators such as `+`: The operands may be lvalues or rvalues, but what bout the result?
- An expression such as `m + n` places its result:
  - not in `m`, not in `n`, but in a compiler-generated temporary object, often a CPU register.
- Such temporary objects are rvalues.
- For example, why this is an error `m + 1 = n;`? Because `m + 1` yields an rvalue!

## Unary `*`

- Unary `*` yields an lvalue.
- A pointer `p` can point to an object, so *p is an lvalue

```cpp
int a[N];
int* p = a;
char* s = nullptr;
*p = 3;          // OK
*s = '\0';       // undefined behavior
```

- Note: Lvalue-ness is a compile-time property. `*s` is an lvalue even if `s` is null.
- If `s` is null, evaluating `*s` causes undefined behavior.

## :exploding_head: Data storage for expressions

- Conceptually, rvalues (of non-class type) don't occupy data storage in the object program.
  - **In truth, some might.** (For example, when the temporary is just too large to fit in to the register)
  - C and C++ insist that you program as if non-class rvalues don't occupy storage.
- Conceptually, lvalues (of any type) occupy data storage.
  - **In truth, the optimizer might eliminate some of them** (But only when you won't notice).
  - C and C++ let you assume that lvalues always do occupy storage

## Rvalues of class type

- Conceptually, rvalues of class type do occupy data storage. Why the difference?
- Consider:

```cpp
struct S {
  int x, yl
};

S s1 = { 1, 4 };

int i = s1.y;
```

- How does compiler generate codee for the `int i = s1.y` assignment? Compiler will actually uses a base (address of `s1`) + offset calculation to access the member `s1.y`

```cpp
int i = s1.y;
```

- Now consider this:

```cpp
S foo();
int j = foo().y;
```

- In this case, compiler still use a base + offset calculation to get the `foo().y`.
- Therefore, the return value of `foo()` must have a base address.
- Conceptually, any object with an address occupies data storage
- This is why rvalues of class types must be treated differently


## Non-modifiable lvalues

- In fact, not all lvalues can appear on the left of an assignment, e.g. it's non-modifiable if it has const-qualified type.

```cpp
char cnost name[] = "dan";
name[0] = 'D'; // error
```

- `name[0]` is an lvalue, but it's non-modifiable.
- Each element of a const array is itself const.
- Lvalues and rvalues provide a vocabulary for describing subtle behavioral difference.
- Such as between enumeration constants and const objects. For example, this `MAX` is a constant of an unnamed enumeration type:

```cpp
enum { MAX = 100 };
```

- Unscoped enumeration values implicitly convert to an int.
- When `MAX` appears in an expression, it yields an integer rvalue. Thus, you can't assign to it:

```cpp
MAX += 3; // error, MAX ia a rvalue
```

- You also can't take its address:

```cpp
int* p = &MAX;  // error again, MAX is a rvalue
```

- On the other hand, this MIN is a const-qualified object

```cpp
int const MIN = 100;
```

- When it appears in an expression, it's a non-modifiable lvalue. Thus, you still can't assign to it.

```cpp
MIN += 3;  // error, MIN is non-modifiable
```

- However, taking address is fine

```cpp
int const * p = &MAX;  // OK, MIN is a lvalue
```

## Reference types

- The concepts of lvalues and rvalues help explain C++ reference type
- References provide an alternative to pointers as way of associating names with objects
- C++ libraries often use references instead of pointers as function parameters and return type.
- Consider the following:

```cpp
int i;
int &ri = i;
```
- The last line:
  - defines `ri` with type "reference to `int`" and
  - initializes `ri` to refer to `i`
- Hence, reference `ri` is an alias for `i`
- **A reference is essentially a pointer that's automatically dereferenced each time it's used.**
- You can rewrite most, if not all, code that uses a references as code that uses a const pointer (as once you bind the reference, you can't change. So it should be a const pointer that can't point to other address once initialized), as in:

```cpp
int& ri = i;  // ---> int * const cpi = &i;
ri = 4;       // *cpi = 4;
int j = ri + 2; // int j = *cpi + 2;
```

- A reference yields an lvalue.

## References and overloaded operators

- What good are references? Why not just use pointers?
- References can provide friendlier function interfaces.
  - C++ has references so that overloaded operators can look just like built-in operators...
  - One philosophy in C/C++ is that built-in types and class type should behave very much the same. If we don't have references, it will be hard to write overloaded operators that gives us such behavior.

```cpp
enum month {
  Jan, Feb, ..., month_end
};

typedef enum month month;

for (month m = Jan; m <= Dec; ++m) {
  // ...
}

- This code compiles and executes as expected in C, but not in C++
- In C++, the build-in `++` wont accept an operand of enumeration type.
- You need to overload `++` for `month`.
- Lets try it without references...

```cpp
void operator++(month x) {        // pass by value
  x = static_cast<month>(x + 1);
}
```

- This compiles, but doesn't increment `m`, it increments a copy of `m` in parameter `x`.
- Also, this implementation lets you apply `++` to an rvalue, as in:

```cpp
++Apr;   // compiles, but shouldn't
```

- The proper overloaded `++` should behave like the built-in `++`, as in:

```cpp
++42; // compile error: can't increment an rvalue!!
```

- We need a `++` that passes in a `month` it cna modify. What if we do:

```cpp
void operator++(month* x) {
  *x = static_cast<month>(*x + 1);
}
```

- In fact, this function definition won't compile. You can't overload an operator with a parameter of pointer type.
- Even if the definition compiled, it wouldn't work like a built-in `++`

```cpp
++m; // looks right but doesn't compile
++&m; // looks wrong (not like built-in ++) and, doesn't compile either
```

- We really need a `++` that can modify a `month` object but without passing explicitly by address:

```cpp
void operator++(month& x) {
  x = static_cast<month>(x + 1);
}
```

- Then we can do  `++m;` as expected, and as a bonus, this `++` operator won't accept an rvalue (just like for built-in type): `++Apr;` yields compile error!
- Actually, a proper prefix++ doesn't return void.
- It returns the incremented object by reference:

```cpp
month& operator++(month& x) {
  return x = static_cast<month>(x + 1);
}
```

## "Reference to const" parameters

- A "reference to const" parameter will accept an argument that's either const or non-const.

```cpp
R f(T const& t);
```

- In contrast, a reference (to non-const) parameter will accept only a non-const argument.
- When it appears in an expression, a "reference to const" yields a non-modifiable lvlaue.
- So the outward behavior is the same as a function declared as by value parameter (the calls look and act very much the same):

```cpp
R f(T t);
```

- E.g. calling `f` just can't alter the actual argument passed in
  - By value, `f` has access only to a copy of `t`, not `t` itself.
  - By "reference to const": f's parameter is declared to be non-modifiable.

## Why use "reference to const"?

- Passing by "reference to const" might be more efficient than passing by value. (It depends on the cost to make a copy).

## References and temporaries

- A "pointer to `T`" can point only an an lvalue of type `T`
- A "reference to `T`" binds only to an lvalue of type `T`

```cpp
int* pi = &3; // can't apply & to 3
int& ri = 3;  // can't bind this either

int i = 0;
double* pd = &i; // can't convert pointers
double& rd = i; // can't bind this, either
```

- There's an exception to the rule that a reference must bind to an lvalue of hte referenced type: A "reference to const `T`" can bind to an expression `e` that's not an lvalue of type `T`, if there's a conversion from `e`'s type to `T`
- In this case, the compiler creates a temporary object to hold a copy of `e` converted to `T`, this is so the reference has something to bind to.
- Given `double const & rd = 3;`, when program execution reaches this declaration, the program:
  - 1. converts the value of 3 from int to double.
  - 2. creates a temporary double to hold the converted result, and
  - 3. binds rd to the temporary.

- When execution leaves the scope containing `rd`, the program:
  - 4. destroys the temporary

- Why do we have this special rules for reference to const `T`? This enables passing by "reference to const" to consistently have the same outward behavior as passing by value.
- For example, compares below:

```cpp
long double x;
void f(long double ld); // by value

f(x);  // passes a copy of x
f(1);  // passes a copy of 1 converted to long double
```

and

```cpp
long double x;
void f(long double const & ld); // by reference to const

f(x);  // passes a reference to x
f(1);  // passes a reference to a temporary containing 1 converted to long double
```

- Either way, the function calls behave the same.

## Two kinds of Rvalues

- Conceptually, rvalues of built-in types don't occupy storage.
- However, **the temporary object created in this way does.**
  - Even if it has a built-in type like `long double`
- The temporary is still an rvalue, but it occupies data storage. Just like rvalues of class types do.
- In modern C++, there are actually 2 kinds fof rvalues.
  - As a programmer, in general, you don't need to worry about this. It's more in standard to define the behavior of language. So compiler authors know what needs to be done.

:one: `prvalues`

- "Pure rvalues", which don't occupy data storage.

:two: `xvalues`

- "Expiring values", which do occupy data storage.
- When temporary object is created through a ***temporary materialization conversion***, **it converts a prvalue into an xvalue.**

## Mimicking built-in operators

- Recall the behavior of the built-in `+` operator:
  - The operands may be lvalues or rvalues.
  - The result is always an rvalue.
- How do you declare an overloaded operator with the same behavior?
- Consider a rudimentary (character) string class with `+` as a concatenation operator... you can declare operator `+` as a non-member as in:

```cpp
class string {
public:
  string(const string&);
  string(const char*); // converting constructor
  string& operator=(const string&);
};

string operator+(const string& lo, const string& ro);
```

- Parameters `lo` and `ro` accept arguments that are either lvalue or rvalue. So we can do:

```cpp
string s = "hello";
string t = "world";
s = s + ", " + t;
```

- Under the hood, the compiler applies the converting constructor implicitly:

```cpp
s = s + string(", ") + t; // lvalue + rvalue + lvalue
```

- The function returns its result by value. Calling this operator `+` yields an rvalue:

```cpp
string* p = &(s + t); // error, can't take the address
```

## Rvalue references

- Hence C++11 introduces another kind of reference:
  - What C++03 calls "references", C++11 calls "lvalue references"
  - This distinguishes them from C++11's new "rvalue references"
- Except for the name change, lvalue references in C++11 behave like references in C++03.

- Whereas an lvalue reference declaration uses the `&` operator, an `rvalue reference` uses the `&&` operator. For example:

```cpp
int&& ri = 10; // rvalue reference to int
```

- You can use "rvalue references" as function parameters and return types, as in:

```cpp
double&& f(int&& ri);
```

- You can also have an "rvalue reference to const", though it's not that useful as rvalue reference is used to support move semantics. Such semantics implies modifying the referenee.
- Rvalue references bind only to rvalue. (This is true even for rvalue reference to const):

```cpp
int n = 10;
int&& ri = n; // error, n is lvalue
int const && rj = n; // error, n is lvalue
```

- Binding an "rvalue reference" to an rvalue triggers a temporary materialization conversion (e.g. converting a prvalue to a xvalue). Just like binding an "lvalue reference to const" to an rvalue.


## Move operations

- Modern C++ uses rvalue references to implement move operations that can avoid unnecessary copying.
- E.g. when you don't care about the moved-from object, then you don't need to copy and move it, you can just move it.

```cpp
class string {
public:
  string(const string&);            // copy ctor
  string& operator=(const string&); // copy assignment

  string(string&&) noexcept;            // move ctor
  string& operator=(string&&) noexcept; // move assignment
};
```

- Given the following objects:

```cpp
string s1, s2, s3;
```

- Assigning from an lvalue results in copy assignment.
- The value isn't expiring, so it must be preserved.

```cpp
s1 = s2; // string& operator=(const string&);
```

- On the other hand, assigning from an rvalue results in move assignment. The value expires at the end of the statement, so it can be safely moved.

```cpp
s1 = s2 + s3; // string& operator=(string&&) noexcept;
```

## Binding an "rvalue reference" to an rvalue creates an xvalue

- However, look inside the move assignment operator:

```cpp
string& string::operator(string&& other) {
  //...
  string temp(other); // this calls string(const string&);
  //...
}
```

- Why it's calling copy ctor? Because within `operator=`, `other` exists for the duration of the function. So in this context, `other` itself is a lvalue!
  - (From caller's perspective, they are still pass rvalue in, but `other` within the function body becomes lvalue.)
  - In general, if it has a name, it's an lvalue.

- Sometimes, it makes sense to move from an lvalue.

```cpp
template <typename T>
void swap(T& a, T& b) {
  T temp(a); // copy ctor
  a = b; // copy assignment
  b = temp; // copy assignment
}
```

- The compiler copy constructs `temp` because `a` is not expiring.
- But we know that the next line overwrites `a` - so there is no need to preserve the value of `a`
- So it's safe to move from an lvalue only if it's expiring. The compiler can't always recognize an expiring lvalue though. How can you inform the compiler?
- To move from an lvalue, **you need to convert it to an xvalue. In other words, convert it to an unnamed rvalue reference.**
- So this is what `std::move` does:

```cpp
template <typename T>
constexpr T&& std::move(T&& a); // simplified version
```

- Return values don't have names. Doing this can move lvalue to xvalue, which is returned as a unnamed rvalue reference.


## Value categories

- Modern C++ introduces a more complex categorization of expressions.

```txt
          expression
        /            \
      glvalue      rvalue
      /     \      /    \
     lvalue   xvalue     prvalue
```

`glvalue`: a **generalized** lvalue

`prvalue`: a **pure** rvalue

`xvalue`: an **expiring** lvalue -> it behaves both like lvalue and rvalue


## QA

- When do lvalue becomes xvalue? Generally in return statement.
- Why you say string literal is lvalue, you can't do `"abc" = "def"`, right? It's because underneath string literal, it's a char array. And you basically can't just do an char array to char array assignment. So compiler bans it. But not that because `"abc"` isn't a lvalue.
