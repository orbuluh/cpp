# F.def: Function definitions
- A function definition is a function declaration that also specifies the function's implementation, the function body.

## F.1: "Package" meaningful operations as carefully named functions
- Factoring out common code makes code more readable, more likely to be reused, and limit errors from complex code.
- If something is a well-specified action, separate it out from its surrounding code and give it a name.

```cpp
// Example, don't
void read_and_print(istream& is) // read and print an int
{
    int x;
    if (is >> x)
        cout << "the int is " << x << '\n';
    else
        cerr << "no int on input\n";
}
```
- Almost everything is wrong with `read_and_print`. It reads, it writes (to a fixed `ostream`), it writes error messages (to a fixed `ostream`), it handles only ints.
- There is nothing to reuse, logically separate operations are intermingled and local variables are in scope after the end of their logical use.
- For a tiny example, this looks OK, but if the input operation, the output operation, and the error handling had been more complicated the tangled mess could become hard to understand.
- Note: **If you write a non-trivial lambda that potentially can be used in more than one place, give it a name by assigning it to a (usually non-local) variable.**
- Example:
```cpp
sort(a, b, [](T x, T y) { return x.rank() < y.rank() && x.value() < y.value(); });
```
- Naming that lambda breaks up the expression into its logical parts and provides a strong hint to the meaning of the lambda.
```cpp
auto lessT = [](T x, T y) { return x.rank() < y.rank() && x.value() < y.value(); };
sort(a, b, lessT);
find_if(a, b, lessT);
```
- The shortest code is not always the best for performance or maintainability.
- Exception: Loop bodies, including lambdas used as loop bodies, rarely need to be named.
- However, large loop bodies (e.g., dozens of lines or dozens of pages) can be a problem. The rule Keep functions short and simple implies "Keep loop bodies short."
- Similarly, lambdas used as callback arguments are sometimes non-trivial, yet unlikely to be reusable.

## F.2: A function should perform a single logical operation
- A function that performs a single operation is simpler to understand, test, and reuse.

```cpp
void read_and_print()    // bad
{
    int x;
    cin >> x;
    // check for errors
    cout << x << "\n";
}
```
- This is a monolith that is tied to a specific input and will never find another (different) use.
- Instead, break functions up into suitable logical parts and parameterize:
```cpp
int read(istream& is) // better
{
    int x;
    is >> x;
    // check for errors
    return x;
}

void print(ostream& os, int x) { os << x << "\n"; }

void read_and_print() {
    auto x = read(cin);
    print(cout, x);
}
```
- If there was a need, we could further templatize read() and print() on the data type, the I/O mechanism, the response to errors, etc. Example:
```cpp
auto read = [](auto& input, auto& value) // better
{
    input >> value;
    // check for errors
};

auto print(auto& output, const auto& value) { output << value << "\n"; }
```

## F.3: Keep functions short and simple
- Large functions are hard to read, more likely to contain complex code, and more likely to have variables in larger than minimal scopes.
- Functions with complex control structures are more likely to be long and more likely to hide logical errors
- Note: "It doesn't fit on a screen" is often a good practical definition of "far too large."
- **One-to-five-line functions should be considered normal.**
- Note: Break large functions up into smaller cohesive and named functions.
- Small simple functions are easily inlined where the cost of a function call is significant.

## F.4: If a function might have to be evaluated at compile time, declare it `constexpr`
- `constexpr` is needed to tell the compiler to allow compile-time evaluation.
- Example: The (in)famous factorial:
```cpp
constexpr int fac(int n)
{
    constexpr int max_exp = 17;      // constexpr enables max_exp to be used in Expects
    Expects(0 <= n && n < max_exp);  // prevent silliness and overflow
    int x = 1;
    for (int i = 2; i <= n; ++i) x *= i;
    return x;
}
```
- This is C++14. For C++11, use a recursive formulation of fac().
- Note: `constexpr` does not guarantee compile-time evaluation; it just guarantees that the function can be evaluated at compile time for constant expression arguments if the programmer requires it or the compiler decides to do so to optimize.
```cpp
constexpr int min(int x, int y) { return x < y ? x : y; }

void test(int v) {
    int m1 = min(-1, 2);           // probably compile-time evaluation
    constexpr int m2 = min(-1, 2); // compile-time evaluation
    int m3 = min(-1, v);           // run-time evaluation
    constexpr int m4 = min(-1, v); // error: cannot evaluate at compile time
}
```
- Note: **Don't try to make all functions `constexpr`**. Most computation is best done at run time.
- Note: Any API that might eventually depend on high-level run-time configuration or business logic should not be made `constexpr`.
- Such customization can not be evaluated by the compiler, and any `constexpr` functions that depended upon that API would have to be refactored or drop `constexpr`.


## F.5: If a function is very small and time-critical, declare it `inline`
- Some optimizers are good at inlining without hints from the programmer, but don't rely on it.
- Measure! Over the last 40 years or so, we have been promised compilers that can `inline` better than humans without hints from humans. We are still waiting.
- Specifying `inline` (**explicitly, or implicitly when writing member functions inside a class definition**) **encourages the compiler to do a better job.**
- Example: `inline string cat(const string& s, const string& s2) { return s + s2; }`
- Exception Do not put an `inline` function in what is meant to be a stable interface **unless you are certain that it will not change. An `inline` function is part of the ABI.**
- Note: `constexpr` implies `inline`.
- Note: Member functions defined in-class are `inline` by default.
- Exception: Function templates (including member functions of class `templates A<T>::function()` and member function templates `A::function<T>())` are normally defined in headers and therefore `inline`.

## F.6: If your function must not throw, declare it `noexcept`
- If an exception is not supposed to be thrown, the program cannot be assumed to cope with the error and should be terminated as soon as possible.
- Declaring a function `noexcept` **helps optimizers by reducing the number of alternative execution paths**.
- It also speeds up the exit after failure.
- Put `noexcept` on every function **written completely in C or in any other language without exceptions.**
- The C++ Standard Library does that implicitly for all functions in the C Standard Library.
- Note: `constexpr` functions can throw when evaluated at run time, so you might need conditional `noexcept` for some of those.
- Example: You can use `noexcept` even on functions that can throw:
```cpp
vector<string> collect(istream& is) noexcept {
    vector<string> res;
    for (string s; is >> s;)
        res.push_back(s);
    return res;
}
```
- If `collect()` runs out of memory, the program crashes.
- Unless the program is crafted to survive memory exhaustion, that might be just the right thing to do; `terminate()` might generate suitable error log information (but after memory runs out it is hard to do anything clever).

- Note: You must be aware of the execution environment that your code is running when deciding whether to tag a function `noexcept`, especially because of the issue of throwing and allocation.
- Code that is intended to be perfectly general (like the standard library and other utility code of that sort) needs to support environments where a `bad_alloc` exception could be handled meaningfully.
- However, **most programs and execution environments cannot meaningfully handle a failure to allocate**, and **aborting the program is the cleanest and simplest response to an allocation failure in those cases.**
  - If you know that your application code cannot respond to an allocation failure, it could be appropriate to add `noexcept` even on functions that allocate.
- Put another way: In most programs, most functions can throw (e.g., because they use new, call functions that do, or use library functions that reports failure by throwing), so **don't just sprinkle `noexcept` all over the place without considering whether the possible exceptions can be handled.**
- `noexcept` is most useful (and most clearly correct) for frequently used, low-level functions.

- Note: Destructors, swap functions, move operations, and default constructors should never throw. See also [C.44](C.ctor.md#c44-prefer-default-constructors-to-be-simple-and-non-throwing).


## F.7: For general use, take `T*` or `T&` arguments rather than smart pointers
- NOT YET READ