# ES: Expressions and statements
- Expressions and statements are the lowest and most direct way of expressing actions and computation. Declarations in local scopes are statements.

## ES.28: Use lambdas for complex initialization, especially of const variables
- It nicely encapsulates local initialization, including cleaning up scratch variables needed only for the initialization, without needing to create a needless non-local yet non-reusable function.
- It also works for variables that should be const but only after some initialization work.
- If at all possible, reduce the conditions to a simple set of alternatives (e.g., an enum) and don't mix up selection and initialization.
```cpp
// Bad
widget x;                               // should be const, but:
for (auto i = 2; i <= N; ++i) {         // this could be some
    x += some_obj.do_something_with(i); // arbitrarily long code
} // needed to initialize x
// from here, x should be const, but we can't say so in code in this style
```
```cpp
// Prefer
const widget x = [&] {
    widget val; // assume that widget has a default constructor
    for (auto i = 2; i <= N; ++i) {           // this could be some
        val += some_obj.do_something_with(i); // arbitrarily long code
    }                                         // needed to initialize x
    return val;
}();
```