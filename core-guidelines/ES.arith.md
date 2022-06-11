# Arithmetic

## ES.100: Don't mix signed and unsigned arithmetic
- Avoid wrong results.

```cppp
int x = -3;
unsigned int y = 7;

cout << x - y << '\n';  // unsigned result, possibly 4294967286
cout << x + y << '\n';  // unsigned result: 4
cout << x * y << '\n';  // unsigned result, possibly 4294967275
```
- It is harder to spot the problem in more realistic examples.
- Note - Unfortunately, C++ uses signed integers for array subscripts and the standard library uses unsigned integers for container subscripts- This precludes consistency. Use gsl::index for subscripts; see [ES.107](#es107-dont-use-unsigned-for-subscripts-prefer-gslindex).


## ES.101: Use unsigned types for bit manipulation
- Unsigned types support bit manipulation without surprises from sign bits.

```cpp
unsigned char x = 0b1010'1010;
unsigned char y = ~x;   // y == 0b0101'0101;
```
- Unsigned types can also be useful for modulo arithmetic. However, if you want modulo arithmetic add comments as necessary noting the reliance on wraparound behavior, as such code can be surprising for many programmers.

## ES.102: Use signed types for arithmetic

- Because most arithmetic is assumed to be signed; x - y yields a negative number when y > x except in the rare cases where you really want modulo arithmetic.
- Unsigned arithmetic can yield surprising results if you are not expecting it. This is even more true for mixed signed and unsigned arithmetic.
```cpp
template<typename T, typename T2>
T subtract(T x, T2 y)
{
    return x - y;
}

void test()
{
    int s = 5;
    unsigned int us = 5;
    cout << subtract(s, 7) << '\n';       // -2
    cout << subtract(us, 7u) << '\n';     // 4294967294
    cout << subtract(s, 7u) << '\n';      // -2
    cout << subtract(us, 7) << '\n';      // 4294967294
    cout << subtract(s, us + 2) << '\n';  // -2
    cout << subtract(us, s + 2) << '\n';  // 4294967294
}
```
- Here we have been very explicit about what's happening, but if you had seen us - (s + 2) or s += 2; ...; us - s, would you reliably have suspected that the result would print as 4294967294?

- Exception: Use unsigned types if you really want modulo arithmetic - add comments as necessary noting the reliance on overflow behavior, as such code is going to be surprising for many programmers.
- The standard library uses unsigned types for subscripts. The built-in array uses signed types for subscripts. This makes surprises (and bugs) inevitable.
```cpp
int a[10];
for (int i = 0; i < 10; ++i)
    a[i] = i;
vector<int> v(10);
// compares signed to unsigned; some compilers warn, but we should not
for (gsl::index i = 0; i < v.size(); ++i)
    v[i] = i;

int a2[-2]; // error: negative size

// OK, but the number of ints (4294967294) is so large that we should get an
// exception
vector<int> v2(-2);
```
- Use `gsl::index` for subscripts; see [ES.107](#es107-dont-use-unsigned-for-subscripts-prefer-gslindex).


## ES.103: Don't overflow
- Overflow usually makes your numeric algorithm meaningless.
- Incrementing a value beyond a maximum value can lead to memory corruption and undefined behavior.

```cpp
int a[10];
a[10] = 7;   // bad, array bounds overflow

for (int n = 0; n <= 10; ++n)
    a[n] = 9;   // bad, array bounds overflow

int n = numeric_limits<int>::max();
int m = n + 1;   // bad, numeric overflow

int area(int h, int w) { return h * w; }
auto a = area(10'000'000, 100'000'000);   // bad, numeric overflow
```
- Exception: Use unsigned types if you really want modulo arithmetic.
- Alternative: For critical applications that can afford some overhead, use a range-checked integer and/or floating-point type.

## ES.104: Don't underflow
- Decrementing a value beyond a minimum value can lead to memory corruption and undefined behavior.

```cpp
int a[10];
a[-2] = 7;   // bad

int n = 101;
while (n--)
    a[n - 1] = 9;   // bad (twice)
```

## ES.105: Don't divide by integer zero
- The result is undefined and probably a crash.
- This also applies to %.
```cpp
// Example, bad
int divide(int a, int b) {
    // BAD, should be checked (e.g., in a precondition)
    return a / b;
}
// Example, good
int divide(int a, int b) {
    // good, address via precondition (and replace with contracts once C++ gets
    // them)
    Expects(b != 0);
    return a / b;
}

double divide(double a, double b) {
    // good, address via using double instead
    return a / b;
}
```
- Alternative: For critical applications that can afford some overhead, use a range-checked integer and/or floating-point type.

## ES.106: Don't try to avoid negative values by using unsigned
- Choosing unsigned implies many changes to the usual behavior of integers, including modulo arithmetic, can suppress warnings related to overflow, and opens the door for errors related to signed/unsigned mixes.
- Using unsigned doesn't actually eliminate the possibility of negative values.

```cpp
// Example
unsigned int u1 = -2;   // Valid: the value of u1 is 4294967294
int i1 = -2;
unsigned int u2 = i1;   // Valid: the value of u2 is 4294967294
int i2 = u2;            // Valid: the value of i2 is -2
```
- These problems with such (perfectly legal) constructs are hard to spot in real code and are the source of many real-world errors. Consider:

```cpp
unsigned area(unsigned height, unsigned width) { return height*width; } // [see also](#Ri-expects)
// ...
int height;
cin >> height;
auto a = area(height, 2);   // if the input is -2 a becomes 4294967292
```
- Remember that -1 when assigned to an unsigned int becomes the largest unsigned int. Also, **since unsigned arithmetic is modulo arithmetic the multiplication didn't overflow, it wrapped around.**

```cpp
// Example
unsigned max = 100000;    // "accidental typo", I mean to say 10'000
unsigned short x = 100;
while (x < max) x += 100; // infinite loop
```
- Had x been a signed short, we could have warned about the undefined behavior upon overflow.

```cpp
struct Positive {
    int val;
    Positive(int x) :val{x} { Assert(0 < x); }
    operator int() { return val; }
};

int f(Positive arg) { return arg; }

int r1 = f(2);
int r2 = f(-2);  // throws
```

## ES.107: Don't use unsigned for subscripts, prefer `gsl::index`
- To avoid signed/unsigned confusion. To enable better optimization. To enable better error detection. To avoid the pitfalls with auto and int.
```cpp
// Example, bad
vector<int> vec = /*...*/;

for (int i = 0; i < vec.size(); i += 2) // might not be big enough
    cout << vec[i] << '\n';
for (unsigned i = 0; i < vec.size(); i += 2) // risk wraparound
    cout << vec[i] << '\n';
for (auto i = 0; i < vec.size(); i += 2) // might not be big enough
    cout << vec[i] << '\n';
for (vector<int>::size_type i = 0; i < vec.size(); i += 2) // verbose
    cout << vec[i] << '\n';
for (auto i = vec.size() - 1; i >= 0; i -= 2) // bug
    cout << vec[i] << '\n';
for (int i = vec.size() - 1; i >= 0; i -= 2) // might not be big enough
    cout << vec[i] << '\n';
```

```cpp
vector<int> vec = /*...*/;

for (gsl::index i = 0; i < vec.size(); i += 2)             // ok
    cout << vec[i] << '\n';
for (gsl::index i = vec.size()-1; i >= 0; i -= 2)          // ok
    cout << vec[i] << '\n';
```
- Note - The built-in array uses signed subscripts. The standard-library containers use unsigned subscripts.
- Thus, no perfect and fully compatible solution is possible (unless and until the standard-library containers change to use signed subscripts someday in the future).
- Given the known problems with unsigned and signed/unsigned mixtures, better stick to (signed) integers of a sufficient size, which is guaranteed by `gsl::index`.

Example
```cpp
template <typename T> struct My_container {
  public:
    // ...
    T& operator[](gsl::index i); // not unsigned
    // ...
};
