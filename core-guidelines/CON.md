# Con: Constants and immutability
- You can't have a race condition on a constant. It is easier to reason about a program when many of the objects cannot change their values.
- Interfaces that promises "no change" of objects passed as arguments greatly increase readability.

# Con.1: By default, make objects immutable
- Immutable objects are easier to reason about, so make objects non-const only when there is a need to change their value.Prevents accidental or hard-to-notice change of value.
- Exception - Function parameters passed by value are rarely mutated, but also rarely declared `const`.
- To avoid confusion and lots of false positives, don't enforce this rule for function parameters.
```cpp
void f(const char* const p); // pedantic
void g(const int i) { ... }  // pedantic
```
- Note that a function parameter is a local variable so changes to it are local.

# Con.2: By default, make member functions const
- A member function should be marked `const` unless it changes the object's observable state.
- This gives a more precise statement of design intent, better readability, more errors caught by the compiler, and sometimes more optimization opportunities.
- Note: It is not inherently bad to pass a pointer or reference to non-const, but that should be done only when the called function is supposed to modify the object.
- A reader of code must assume that a function that takes a "plain" T* or T& will modify the object referred to.
- If it doesn't now, it might do so later without forcing recompilation.
- Note: There are code/libraries that offer functions that declare a `T*` even though those functions do not modify that `T`.
- This is a problem for people modernizing code.
- You can update the library to be const-correct; preferred long-term solution "cast away const";best avoided provide a wrapper function

```cpp
// example
void f(int* p);   // old code: f() does not modify `*p`
void f(const int* p) { f(const_cast<int*>(p)); } // wrapper
```
- Note that this wrapper solution is a patch that should be used only when the declaration of `f()` cannot be modified, e.g. because it is in a library that you cannot modify.
- Note: A const member function can modify the value of an object that is `mutable` **or accessed through a pointer member**.
  - A common use is to maintain a cache rather than repeatedly do a complicated computation.
  - For example, here is a Date that caches (memoizes) its string representation to simplify repeated uses:
```cpp
class Date {
  public:
    // ...
    const string& string_ref() const {
        if (string_val == "")
            compute_string_rep();
        return string_val;
    }
    // ...
  private:
    void compute_string_rep()
        const; // compute string representation and place it in string_val
    mutable string string_val;
    // ...
};
```
- Another way of saying this is that **constness is not transitive**. It is possible for a `const` member function to change the value of mutable members and the value of objects accessed **through non-const pointers**.
  - It is the job of the class to ensure such mutation is done only when it makes sense according to the semantics (invariants) it offers to its users.

## Con.3: By default, pass pointers and references to consts
- To avoid a called function unexpectedly changing the value.
- It's far easier to reason about programs when called functions don't modify state.

```cpp
void f(char* p);        // does f modify *p? (assume it does)
void g(const char* p);  // g does not modify *p
```
- Note: It is not inherently bad to pass a pointer or reference to non-const, but that should be done only when the called function is supposed to modify the object.
- Note: Do not cast away const.

## Con.4: Use const to define objects with values that do not change after construction
- Prevent surprises from unexpectedly changed object values.
```cpp
//Example
void f() {
    int x = 7;
    const int y = 9;

    for (;;) {
        // ...
    }
    // ...
}
```
- As x is not const, we must assume that it is modified somewhere in the loop.

## Con.5: Use constexpr for values that can be computed at compile time
- Better performance, better compile-time checking, guaranteed compile-time evaluation, no possibility of race conditions.

```cpp
double x = f(2);            // possible run-time evaluation
const double y = f(2);      // possible run-time evaluation
constexpr double z = f(2);  // error unless f(2) can be evaluated at compile time
```
