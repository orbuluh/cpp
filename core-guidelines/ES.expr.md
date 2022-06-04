# ES.expr: Expressions

## ES.40: Avoid complicated expressions
- Complicated expressions are error-prone.
```cpp
// bad: assignment hidden in subexpression
while ((c = getc()) != -1)

// bad: two non-local variables assigned in sub-expressions
while ((cin >> c1, cin >> c2), c1 == c2)

// better, but possibly still too complicated
for (char c1, c2; cin >> c1 >> c2 && c1 == c2;)

// OK: if i and j are not aliased
int x = ++i + ++j;

// OK: if i != j and i != k
v[i] = v[j] + v[k];

// bad: multiple assignments "hidden" in subexpressions
x = a + (b = f()) + (c = g()) * 7;

// bad: relies on commonly misunderstood precedence rules
x = a & b + c * d && e ^ f == 7;

// bad: undefined behavior
x = x++ + x++ + ++x;
```
- Some of these expressions are unconditionally bad (e.g., they rely on undefined behavior).
- Others are simply so complicated and/or unusual that even good programmers could misunderstand them or overlook a problem when in a hurry.
- C++17 tightens up the rules for the order of evaluation (**left-to-right except right-to-left in assignments, and the order of evaluation of function arguments is unspecified**; see ES.43), but that doesn't change the fact that complicated expressions are potentially confusing.
- Note: A programmer should know and use the basic rules for expressions.
```cpp
x = k * y + z;             // OK

auto t1 = k * y;           // bad: unnecessarily verbose
x = t1 + z;

if (0 <= x && x < max)   // OK

auto t1 = 0 <= x;        // bad: unnecessarily verbose
auto t2 = x < max;
if (t1 && t2)            // ...
```

## ES.41: If in doubt about operator precedence, parenthesize
- Avoid errors. Readability. Not everyone has the operator table memorized.

```cpp
const unsigned int flag = 2;
unsigned int a = flag;

if (a & flag != 0)  // bad: means a&(flag != 0)
```
- Note: We recommend that programmers know their precedence table for the arithmetic operations, the logical operations, but consider mixing bitwise logical operations with other operators in need of parentheses.
```cpp
if ((a & flag) != 0)  // OK: works as intended
```
- You should know enough not to need parentheses for:
```cpp
if (a < 0 || a <= max) {
    // ...
}
```

## ES.42: Keep use of pointers simple and straightforward
- Complicated pointer manipulation is a major source of errors.
- Note: Use `gsl::span` instead. Pointers should only refer to single objects.
- Pointer arithmetic is fragile and easy to get wrong, the source of many, many bad bugs and security violations.
- `span` is a bounds-checked, safe type for accessing arrays of data.
  - Access into an array with known bounds using a constant as a subscript can be validated by the compiler.

```cpp
void f(int* p, int count) {
    if (count < 2)
        return;

    int* q = p + 1; // BAD

    ptrdiff_t d;
    int n;
    d = (p - &n); // OK
    d = (q - p);  // OK

    int n = *p++; // BAD

    if (count < 6)
        return;

    p[4] = 1; // BAD

    p[count - 1] = 2; // BAD

    use(&p[0], 3); // BAD
}
```
```cpp
//Example, good
void f(span<int> a) // BETTER: use span in the function declaration
{
    if (a.size() < 2)
        return;

    int n = a[0]; // OK

    span<int> q = a.subspan(1); // OK

    if (a.size() < 6)
        return;

    a[4] = 1; // OK

    a[a.size() - 1] = 2; // OK

    use(a.data(), 3); // OK
}
```
- Note: Subscripting with a variable is difficult for both tools and humans to validate as safe. span is a run-time bounds-checked, safe type for accessing arrays of data.
- `at()` is another alternative that ensures single accesses are bounds-checked.
- If iterators are needed to access an array, use the iterators from a span constructed over the array.

```cpp
//Example, bad
void f(array<int, 10> a, int pos) {
    a[pos / 2] = 1; // BAD
    a[pos - 1] = 2; // BAD
    a[-1] = 3; // BAD (but easily caught by tools) -- no replacement, just don't
               // do this
    a[10] = 4; // BAD (but easily caught by tools) -- no replacement, just don't
               // do this
}
```
```cpp

// Use a span (good):
void f1(span<int, 10> a, int pos) // A1: Change parameter type to use span
{
    a[pos / 2] = 1; // OK
    a[pos - 1] = 2; // OK
}

void f2(array<int, 10> arr, int pos) // A2: Add local span and use that
{
    span<int> a = {arr.data(), pos};
    a[pos / 2] = 1; // OK
    a[pos - 1] = 2; // OK
}

// Use at() (good):
void f3(array<int, 10> a, int pos) // ALTERNATIVE B: Use at() for access
{
    at(a, pos / 2) = 1; // OK
    at(a, pos - 1) = 2; // OK
}

// Example, bad
void f() {
    int arr[COUNT];
    for (int i = 0; i < COUNT; ++i)
        arr[i] = i; // BAD, cannot use non-constant indexer
}


// Use a span (good):
void f1() {
    int arr[COUNT];
    span<int> av = arr;
    for (int i = 0; i < COUNT; ++i)
        av[i] = i;
}

// Use a span and range-for (good):
void f1a() {
    int arr[COUNT];
    span<int, COUNT> av = arr;
    int i = 0;
    for (auto& e : av)
        e = i++;
}

// Use at() (good):

void f2() {
    int arr[COUNT];
    for (int i = 0; i < COUNT; ++i)
        at(arr, i) = i;
}

// Use a range-for (good):
void f3() {
    int arr[COUNT];
    int i = 0;
    for (auto& e : arr)
        e = i++;
}
```
- Note: Tooling can offer rewrites of array accesses that involve dynamic index expressions to use `at()` instead:
```cpp
static int a[10];

void f(int i, int j) {
    a[i + j] = 12;     // BAD, could be rewritten as ...
    at(a, i + j) = 12; // OK -- bounds-checked
}
```
- Turning an array into a pointer (as the language does essentially always) removes opportunities for checking, so avoid it

```cpp
void g(int* p);

void f() {
    int a[5];
    g(a);     // BAD: are we trying to pass an array?
    g(&a[0]); // OK: passing one object
}
```
- If you want to pass an array, say so:
```cpp
void g(int* p, size_t length); // old (dangerous) code

void g1(span<int> av); // BETTER: get g() changed.

void f2() {
    int a[5];
    span<int> av = a;

    g(av.data(), av.size()); // OK, if you have no choice
    g1(a); // OK -- no decay here, instead use implicit span ctor
}
```


## ES.43: Avoid expressions with undefined order of evaluation
- You have no idea what such code does. Portability. Even if it does something sensible for you, it might do something different on another compiler (e.g., the next release of your compiler) or with a different optimizer setting.
- Note: C++17 tightens up the rules for the order of evaluation: left-to-right except right-to-left in assignments, and the order of evaluation of function arguments is unspecified.
- However, remember that your code might be compiled with a pre-C++17 compiler (e.g., through cut-and-paste) so don't be too clever.
```cpp
v[i] = ++i;   //  the result is undefined
```
- A good rule of thumb is that **you should not read a value twice in an expression where you write to it.**

## ES.44: Don't depend on order of evaluation of function arguments
- Because that order is unspecified.
- C++17 tightens up the rules for the order of evaluation, but the order of evaluation of function arguments is still unspecified.

```cpp
int i = 0;
f(++i, ++i);
```
- Before C++17, the behavior is undefined, so the behavior could be anything (e.g., f(2, 2)).
- Since C++17, this code does not have undefined behavior, but it is still not specified which argument is evaluated first.
- The call will be f(1, 2) or f(2, 1), but you don't know which.

```cpp
f1()->m(f2());          // m(f1(), f2())
cout << f1() << f2();   // operator<<(operator<<(cout, f1()), f2())
```
- Overloaded operators can lead to order of evaluation problems:
  - In C++17, these examples work as expected (left to right) and assignments are evaluated right to left (just as ='s binding is right-to-left)
```cpp
f1() = f2();    // undefined behavior in C++14; in C++17, f2() is evaluated before f1()
```

# ES.45: Avoid "magic constants"; use symbolic constants
- Unnamed constants embedded in expressions are easily overlooked and often hard to understand:
```cpp
for (int m = 1; m <= 12; ++m)   // don't: magic constant 12
    cout << month[m] << '\n';
```
- No, we don't all know that there are 12 months, numbered 1..12, in a year. Better:
```cpp
// months are indexed 1..12
constexpr int first_month = 1;
constexpr int last_month = 12;

for (int m = first_month; m <= last_month; ++m)   // better
    cout << month[m] << '\n';
```
- Better still, don't expose constants:
```cpp
for (auto m : month)
    cout << m << '\n';
```


## ES.46: Avoid lossy (narrowing, truncating) arithmetic conversions
- A narrowing conversion destroys information, often unexpectedly so.

```cpp
// A key example is basic narrowing (bad):

double d = 7.9;
int i = d;  // bad: narrowing: i becomes 7
i = (int)d; // bad: we're going to claim this is still not explicit enough

void f(int x, long y, double d) {
    char c1 = x; // bad: narrowing
    char c2 = y; // bad: narrowing
    char c3 = d; // bad: narrowing
}
```

- Note: The guidelines support library offers a `narrow_cast` operation for specifying that narrowing is acceptable and a narrow ("narrow if") that throws an exception if a narrowing would throw away legal values:
```cpp
i = gsl::narrow_cast<int>(d);   // OK (you asked for it): narrowing: i becomes 7
i = gsl::narrow<int>(d);        // OK: throws narrowing_error
```
- We also include lossy arithmetic casts, such as from a negative floating point type to an unsigned integral type:
```
double d = -7.9;
unsigned u = 0;

u = d;                               // bad: narrowing
u = gsl::narrow_cast<unsigned>(d);   // OK (you asked for it): u becomes 4294967289
u = gsl::narrow<unsigned>(d);        // OK: throws narrowing_error
```
- Note: This rule does not apply to **contextual** conversions to bool:

```cpp
if (ptr) do_something(*ptr);   // OK: ptr is used as a condition
bool b = ptr;                  // bad: narrowing
```
