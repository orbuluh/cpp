# P: Philosophy

- Philosophical rules are generally not mechanically checkable. However, individual rules reflecting these philosophical themes are.
- Without a philosophical basis, the more concrete/specific/checkable rules lack rationale.

## P.1: Express ideas directly in code
- Compilers don't read comments (or design documents) and neither do many programmers (consistently).
- What is expressed in code has defined semantics and can (in principle) be checked by compilers and other tools.

```cpp
class Date {
public:
    Month month() const;  // do
    int month();          // don't
    // ...
};
```
- The first declaration of month is explicit about returning a `Month` and about not modifying the state of the `Date` object.
- The second version leaves the reader guessing and opens more possibilities for uncaught bugs.
- A well-designed library expresses **intent** (**what is to be done,** rather than just how something is being done) far better than direct use of language features.

- A C++ programmer should know the **basics of the standard library**, and use it where appropriate.
- Any programmer should know the **basics of the foundation libraries** of the project being worked on, and use them appropriately.
- Any programmer using these guidelines should know the guidelines support library, and use it appropriately.

```cpp
//Example
change_speed(double s);   // bad: what does s signify?
// ...
change_speed(2.3);
```
- A better approach is to be explicit about the meaning of the double (new speed or delta on old speed?) and the unit used:
```cpp
change_speed(Speed s);    // better: the meaning of s is specified
// ...
change_speed(2.3);        // error: no unit
change_speed(23_m / 10s);  // meters per second
```
- We could have accepted a plain (unit-less) double as a delta, but that would have been error-prone. If we wanted both absolute speed and deltas, we would have defined a `Delta` type.

## P.2: Write in ISO Standard C++
- This is a set of guidelines for writing ISO Standard C++.
- Note: There are environments where extensions are necessary, e.g., to access system resources. In such cases, localize the use of necessary extensions and control their use with non-core Coding Guidelines.
- If possible, build interfaces that encapsulate the extensions so they can be turned off or compiled away on systems that do not support those extensions.
- Extensions often do not have rigorously defined semantics.
  - Even extensions that are common and implemented by multiple compilers might have slightly different behaviors and edge case behavior as a direct result of not having a rigorous standard definition.
  - With sufficient use of any such extension, expected portability will be impacted.
- Note: Using valid ISO C++ does not guarantee portability (let alone correctness). **Avoid dependence on undefined behavior** (e.g., undefined order of evaluation) and **be aware of constructs with implementation defined meaning** (e.g., `sizeof(int)`).
- Note: There are environments where restrictions on use of standard C++ language or library features are necessary, e.g., to avoid dynamic memory allocation as required by aircraft control software standards. In such cases, control their (dis)use with an extension of these Coding Guidelines customized to the specific environment.

## P.3: Express intent
- Unless the intent of some code is stated (e.g., in names or comments), it is impossible to tell whether the code does what it is supposed to do.
```cpp
// bad
gsl::index i = 0;
while (i < v.size()) {
    // ... do something with v[i] ...
}
```
- The intent of "just" looping over the elements of v is not expressed here. The implementation detail of an index is exposed (so that it might be misused), and i outlives the scope of the loop, which might or might not be intended. The reader cannot know from just this section of code.
```cpp
//Better:
for (const auto& x : v) { /* do something with the value of x */ }
```
- Now, there is no explicit mention of the iteration mechanism, and the loop operates on a reference to const elements so that accidental modification cannot happen. If modification is desired, say so:
```cpp
for (auto& x : v) { /* modify x */ }
```
- For more details about for-statements, see ES.71. Sometimes better still, use a named algorithm.
- This example uses the `for_each` from the Ranges TS because it directly expresses the intent:
```cpp
for_each(v, [](int x) { /* do something with the value of x */ });
for_each(par, v, [](int x) { /* do something with the value of x */ });
```
- The last variant makes it clear that we are not interested in the order in which the elements of `v` are handled.
- Note: Alternative formulation: **Say what should be done, rather than just how it should be done.**
- Note: Some language constructs express intent better than others.
```cpp
//Example: If two ints are meant to be the coordinates of a 2D point, say so:
draw_line(int, int, int, int);  // obscure
draw_line(Point, Point);        // clearer
```

## P.4: Ideally, a program should be statically type safe
- Ideally, a program would be completely statically (compile-time) type safe. Unfortunately, that is not possible. Problem areas:
```cpp
unions
casts
array decay
range errors
narrowing conversions
```
- Note: These areas are sources of serious problems (e.g., crashes and security violations). We try to provide alternative techniques.
- Always suggest an alternative. For example:
  - unions -- use variant (in C++17)
  - casts -- minimize their use; templates can help
  - array decay -- use span (from the GSL)
  - range errors -- use span
  - narrowing conversions -- minimize their use and use narrow or narrow_cast (from the GSL) where they are necessary

## P.5: Prefer compile-time checking to run-time checking
- Code clarity and performance. You don't need to write error handlers for errors caught at compile time.

```cpp
// Int is an alias used for integers
int bits = 0;         // don't: avoidable code
for (Int i = 1; i; i <<= 1)
    ++bits;
if (bits < 32)
    cerr << "Int too small\n";
```
- This example fails to achieve what it is trying to achieve (because overflow is undefined) and should be replaced with a simple `static_assert`:
```cpp
// Int is an alias used for integers
static_assert(sizeof(Int) >= 4);    // do: compile-time check
```
- Or better still just use the type system and replace Int with `int32_t`.

```cpp
void read(int* p, int n);   // read max n integers into *p

int a[100];
read(a, 1000);    // bad, off the end
```

- better
```cpp
void read(span<int> r); // read into the range of integers r

int a[100];
read(a);        // better: let the compiler figure out the number of elements
```
- Alternative formulation: Don't postpone to run time what can be done well at compile time.


## P.6: What cannot be checked at compile time should be checkable at run time
- Leaving hard-to-detect errors in a program is asking for crashes and bad results.
- Note: Ideally, we catch all errors (that are not errors in the programmer's logic) at either compile time or run time. It is impossible to catch all errors at compile time and often not affordable to catch all remaining errors at run time.
- However, we should endeavor to **write programs that in principle can be checked**, given sufficient resources (analysis programs, run-time checks, machine resources, time).

```cpp
// Example, bad
// separately compiled, possibly dynamically loaded
extern void f(int* p);

void g(int n) {
    // bad: the number of elements is not passed to f()
    f(new int[n]);
}
```
- Here, a crucial bit of information (the number of elements) has been so thoroughly "obscured" that static analysis is probably rendered infeasible and dynamic checking can be very difficult when `f()` is part of an ABI so that we cannot "instrument" that pointer.
- We could embed helpful information into the free store, but that requires global changes to a system and maybe to the compiler. What we have here is a design that makes error detection very hard.
- Example, bad: We can of course pass the number of elements along with the pointer:

```cpp
// separately compiled, possibly dynamically loaded
extern void f2(int* p, int n);

void g2(int n) {
    f2(new int[n], m); // bad: a wrong number of elements can be passed to f()
}
```
- Passing the number of elements as an argument is better (and far more common) than just passing the pointer and relying on some (unstated) convention for knowing or discovering the number of elements. However (as shown), a simple typo can introduce a serious error. The connection between the two arguments of f2() is conventional, rather than explicit.

- Also, it is implicit that f2() is supposed to delete its argument (or did the caller make a second mistake?).
-Example, bad: The standard library resource management pointers fail to pass the size when they point to an object:
```cpp
// separately compiled, possibly dynamically loaded
// NB: this assumes the calling code is ABI-compatible, using a
// compatible C++ compiler and the same stdlib implementation
extern void f3(unique_ptr<int[]>, int n);

void g3(int n)
{
    f3(make_unique<int[]>(n), m);    // bad: pass ownership and size separately
}
```
- Example: We need to pass the pointer and the number of elements as an integral object:
```cpp
extern void f4(vector<int>&);   // separately compiled, possibly dynamically loaded
extern void f4(span<int>);      // separately compiled, possibly dynamically loaded
                                // NB: this assumes the calling code is ABI-compatible, using a
                                // compatible C++ compiler and the same stdlib implementation

void g3(int n)
{
    vector<int> v(n);
    f4(v);                     // pass a reference, retain ownership
    f4(span<int>{v});          // pass a view, retain ownership
}
```
- This design carries the number of elements along as an integral part of an object, so that errors are unlikely and dynamic (run-time) checking is always feasible, if not always affordable.
- Example: How do we transfer both ownership and all information needed for validating use?
```cpp
vector<int> f5(int n) // OK: move
{
    vector<int> v(n);
    // ... initialize v ...
    return v;
}

unique_ptr<int[]> f6(int n) // bad: loses n
{
    auto p = make_unique<int[]>(n);
    // ... initialize *p ...
    return p;
}

owner<int*> f7(int n) // bad: loses n and we might forget to delete
{
    owner<int*> p = new int[n];
    // ... initialize *p ...
    return p;
}
```

