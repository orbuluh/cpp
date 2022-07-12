# F.call: Parameter passing

## F.15: Prefer simple and conventional ways of passing information
- Using "unusual and clever" techniques causes surprises, slows understanding by other programmers, and encourages bugs.
- If you really feel the need for an optimization beyond the common techniques, measure to ensure that it really is an improvement, and document/comment because the improvement might not be portable.
- The following tables summarize the advice in the following Guidelines, F.16-21.

- [Normal parameter passing](https://github.com/isocpp/CppCoreGuidelines/raw/master/param-passing-normal.png)
- [Advanced parameter passing](https://github.com/isocpp/CppCoreGuidelines/raw/master/param-passing-advanced.png)
  - Use the advanced techniques only after demonstrating need, and document that need in a comment.
- cheap (e.g. `int`) or impossible to copy (e.g. `unique_ptr`)
  - In: `f(X)`
  - In & retain "copy": `f(X)`
  - Out: `X f()`
  - In/Out: `f(X&)`
  - In & move from: `f(X&&)`

- cheap to move (e.g. `std::string`, `std::vector<T>`), or moderate cost to move (e.g. `std::array<std::vector>`, `BigPOD`) or don't know (unfamiliar type, template...)
  - In: `f(const X&)`
  - In & retain "copy": `f(const X&)` + `f(X&&)` & move
  - Out: `X f()`
  - In/Out: `f(X&)`
  - In & move from: `f(X&&)`

- Expensive to move (e.g. `BidPOD[`], `std::array<BigPOD, N>`)
  - In: `f(const X&)`
  - In & retain "copy": `f(const X&)`
  - Out: `f(X&)`
  - In/Out: `f(X&)`
  - In & move from: `f(X&&)`
- For passing sequences of characters see [String](SL.str.md).

## F.16: For "in" parameters, pass cheaply-copied types by value and others by reference to `const`
- Both let the caller know that **a function will not modify the argument**, and **both allow initialization by rvalues**.
- What is "cheap to copy" depends on the machine architecture, but **two or three words (`double`s, pointers, references)** are usually best passed by value.
- **When copying is cheap, nothing beats the simplicity and safety of copying**, and for **small objects (up to two or three words) it is also faster than passing by reference because it does not require an extra indirection to access from the function.**

```cpp
void f1(const string& s); // OK: pass by reference to const; always cheap
void f2(string s); // bad: potentially expensive
void f3(int x); // OK: Unbeatable
void f4(const int& x); // bad: overhead on access in f4()
```
- For advanced uses (only), where you really need to optimize for rvalues passed to "input-only" parameters:
  - If the function is going to unconditionally move from the argument, take it by `&&.` See [F.18](#f18-for-will-move-from-parameters-pass-by-x-and-stdmove-the-parameter).
  - If the function is going to keep a copy of the argument, in addition to passing by `const&` (for lvalues), add an overload that passes the parameter by `&&` (for rvalues) and in the body `std::move`s it to its destination. Essentially this overloads a "will-move-from"; see [F.18](#f18-for-will-move-from-parameters-pass-by-x-and-stdmove-the-parameter).
  - In special cases, such as multiple "input + copy" parameters, consider using perfect forwarding. See [F.19]().

```cpp
int multiply(int, int); // just input ints, pass by value
// suffix is input-only but not as cheap as an int, pass by const&
string& concatenate(string&, const string& suffix);
void sink(unique_ptr<widget>);  // input only, and moves ownership of the widget
```
- Avoid "esoteric techniques" such as passing arguments as T&& "for efficiency". Most rumors about performance advantages from passing by `&&` are false or brittle (but see F.18 and F.19).
- Notes: **A reference can be assumed to refer to a valid object (language rule)**.
  - There is no (legitimate) "null reference."
  - If you need the notion of an `optional` value, use a pointer, `std::optional`, or a special value used to denote "no value."

## F.17: For "in-out" parameters, pass by reference to non-const
- This makes it clear to callers that the object is assumed to be modified.
```cpp
void update(Record& r);  // assume that update writes to r
```
- Note: A `T&` argument can pass information into a function as well as out of it. Thus `T&` could be an in-out-parameter.
- That can in itself be a problem and a source of errors:

```cpp
void f(string& s)
{
    s = "New York";  // non-obvious error
}

void g()
{
    string buffer = ".................................";
    f(buffer);
    // ...
}
```
- Here, the writer of g() is supplying a buffer for `f()` to fill, but `f()` simply replaces it (at a somewhat higher cost than a simple copy of the characters).
- A bad logic error can happen if the writer of `g()` incorrectly assumes the size of the buffer.

## F.18: For "will-move-from" parameters, pass by `X&&` and `std::move` the parameter
- It's efficient and eliminates bugs at the call site: `X&&` binds to rvalues, which requires an explicit `std::move` at the call site if passing an lvalue.

```cpp
void sink(vector<int>&& v)  // sink takes ownership of whatever the argument owned
{
    // usually there might be const accesses of v here
    store_somewhere(std::move(v));
    // usually no more use of v here; it is moved-from
}
```
- Note that the `std::move(v)` makes it possible for `store_somewhere()` to leave `v` in a **moved-from state**. That could be dangerous.
- Exception: Unique owner types that are move-only and cheap-to-move, such as `unique_ptr`, can also be passed by value which is simpler to write and achieves the same effect.
- **Passing by value does generate one extra (cheap) move operation, but prefer simplicity and clarity first.**


```cpp
template<class T>
void sink(std::unique_ptr<T> p)
{
    // use p ... possibly std::move(p) onward somewhere else
}   // p gets destroyed
```

## F.19: For "forward" parameters, pass by `TP&&` and only `std::forward` the parameter
- If the object is t**o be passed onward to other code and not directly used by this function**, we want to make this function agnostic to the argument const-ness and rvalue-ness.

- **In that case, and only that case**, make the parameter `TP&&` where **`TP` is a template type parameter** -- it both ignores and preserves const-ness and rvalue-ness.
- Therefore any code that uses a `TP&& `is implicitly **declaring that it itself doesn't care about the variable's const-ness and rvalue-ness** (because it is ignored), but that intends to pass the value onward to other code that does care about const-ness and rvalue-ness (because it is preserved).
- When used as a parameter `TP&&` is safe because any temporary objects passed from the caller will live for the duration of the function call.
- A parameter of type `TP&&` should essentially always be passed onward via `std::forward` in the body of the function.

```cpp
template<class F, class... Args>
inline auto invoke(F f, Args&&... args)
{
    return f(forward<Args>(args)...);
}
```

## F.20: For "out" output values, prefer return values to output parameters
- A return value is self-documenting, whereas a `&` could be either in-out or out-only and is liable to be misused.
- This includes large objects like standard containers that use implicit move operations for performance and to avoid explicit memory management.
- If you have multiple values to return, use a `tuple` or similar multi-member type.
```cpp
// Example OK: return pointers to elements with the value x
vector<const int*> find_all(const vector<int>&, int x);

// Bad: place pointers to elements with value x in-out
void find_all(const vector<int>&, vector<const int*>& out, int x);
```
- Note: **A struct of many (individually cheap-to-move) elements might be in aggregate expensive to move.**
- Note: "It is not recommended to return a const value. - Such older advice is now obsolete; it does not add value, and it interferes with move semantics.
```cpp
const vector<int> fct();    // bad: that "const" is more trouble than it is worth

void g(vector<int>& vx)
{
    // ...
    fct() = vx;   // prevented by the "const"
    // ...
    vx = fct(); // expensive copy: move semantics suppressed by the "const"
    // ...
}
```
- The argument for adding `const` to a return value is that it prevents (very rare) accidental access to a temporary.
- The argument against is that **it prevents (very frequent) use of move semantics.**

- Exceptions:
  - For non-concrete types, such as types in an inheritance hierarchy, return the object by `unique_ptr` or `shared_ptr`.
  - If a type is expensive to move (e.g., `array<BigPOD>`), consider allocating it on the free store and return a handle (e.g., `unique_ptr`), or passing it in a reference to non-const target object to fill (to be used as an out-parameter).
  - To reuse an object that carries capacity (e.g., `std::string`, `std::vector`) across multiple calls to the function in an inner loop: treat it as an in/out parameter and pass by reference.
- Example: Assuming that Matrix has move operations (possibly by keeping its elements in a `std::vector`):
```cpp
Matrix operator+(const Matrix& a, const Matrix& b)
{
    Matrix res;
    // ... fill res with the sum ...
    return res;
}

Matrix x = m1 + m2;  // move constructor

y = m3 + m3;         // move assignment
```
- Note: The return value optimization doesn't handle the assignment case, but the move assignment does.
```cpp
Example
struct Package {      // exceptional case: expensive-to-move object
    char header[16];
    char load[2024 - 16];
};

Package fill();       // Bad: large return value
void fill(Package&);  // OK

int val();            // OK
void val(int&);       // Bad: Is val reading its argument
```

## F.21: To return multiple "out" values, prefer returning a `struct` or `tuple`
- A return value is self-documenting as an "output-only" value. Note that C++ does have multiple return values, by convention of using a `tuple` (including `pair`), possibly with the extra convenience of `tie` or structured bindings (C++17) at the call site.
- Prefer using a named struct where there are semantics to the returned value. Otherwise, a nameless tuple is useful in generic code.

```cpp
// Example
// BAD: output-only parameter documented in a comment
int f(const string& input, /*output only*/ string& output_data) {
    // ...
    output_data = something();
    return status;
}

// GOOD: self-documenting
tuple<int, string> f(const string& input) {
    // ...
    return make_tuple(status, something());
}
```
- C++98's standard library already used this style, because a pa`ir is like a two-element tuple. For example, given a `set<string> my_set`, consider:

```cpp
// C++98
result = my_set.insert("Hello");
if (result.second) do_something_with(result.first);    // workaround
```
- With C++11 we can write this, putting the results directly in existing local variables:
```cpp
Sometype iter;                                // default initialize if we haven't already
Someothertype success;                        // used these variables for some other purpose

tie(iter, success) = my_set.insert("Hello");   // normal return value
if (success) do_something_with(iter);
```
- With C++17 we are able to use "structured bindings" to declare and initialize the multiple variables:
```cpp
if (auto [ iter, success ] = my_set.insert("Hello"); success) do_something_with(iter);
```
- Exception: Sometimes, we need to pass an object to a function to manipulate its state.
  - In such cases, passing the object by reference `T&` is usually the right technique.
  - Explicitly passing an in-out parameter back out again as a return value is often not necessary.
- For example:

```cpp
istream& operator>>(istream& is, string& s);    // much like std::operator>>()

for (string s; cin >> s; ) {
    // do something with line
}
```
- Here, both s and cin are used as in-out parameters. We pass `cin` by (non-const) reference to be able to manipulate its state.
- We pass `s` to avoid repeated allocations.
- By reusing `s (passed by reference), we allocate new memory only when we need to expand s's capacity.
- This technique is sometimes called the **"caller-allocated out" pattern** and is particularly useful for types, such as `string` and `vector`, that needs to do free store allocations.
- To compare, if we passed out all values as return values, we would something like this:

```cpp
pair<istream&, string> get_string(istream& is)  // not recommended
{
    string s;
    is >> s;
    return {is, s};
}

for (auto p = get_string(cin); p.first; ) {
    // do something with p.second
}
```
- We consider that significantly less elegant with significantly less performance.
- For a truly strict reading of this rule (F.21), the exception isn't really an exception because it relies on in-out parameters, rather than the plain out parameters mentioned in the rule. However, we prefer to be explicit, rather than subtle.
- Note: In many cases, it can be useful to return a specific, user-defined type. For example:

```cpp
struct Distance {
    int value;
    int unit = 1;   // 1 means meters
};

Distance d1 = measure(obj1);        // access d1.value and d1.unit
auto d2 = measure(obj2);            // access d2.value and d2.unit
auto [value, unit] = measure(obj3); // access value and unit; somewhat redundant
                                    // to people who know measure()
auto [x, y] = measure(obj4);        // don't; it's likely to be confusing
```
- The overly-generic `pair` and `tuple` s**hould be used only when the value returned represents independent entities rather than an abstraction.**
- Another example, use a specific type along the lines of `variant<T, error_code>`, rather than using the generic `tuple`.

## F.60: Prefer `T*` over `T&` when "no argument" is a valid option
- A pointer (`T*`) can be a `nullptr` and a `reference (T&)` cannot, there is no valid "null reference".
- Sometimes having `nullptr` as an alternative to indicated "no object" is useful, but if it is not, a reference is notationally simpler and might yield better code.

```cpp
string zstring_to_string(zstring p) // zstring is a char*; that is a C-style string
{
    if (!p) return string{};    // p might be nullptr; remember to check
    return string{p};
}

void print(const vector<int>& r)
{
    // r refers to a vector<int>; no check needed
}
```
- Note: It is possible, but **not valid C++ to construct a reference that is essentially a `nullptr`** (e.g., T* p = nullptr; T& r = *p;). That error is very uncommon.
- Note: If you prefer the pointer notation (`->` and/or `*` vs. `.`), `not_null<T*>` provides the same guarantee as `T&`.


## F.22: Use `T*` or `owner<T*>` to designate a single object
- Readability: it makes the meaning of a plain pointer clear. Enables significant tool support.
- In traditional C and C++ code, plain `T*` is used for many weakly-related purposes, such as:
  - Identify a (single) object (not to be deleted by this function)
  - Point to an object allocated on the free store (and delete it later)
  - Hold the nullptr
  - Identify a C-style string (zero-terminated array of characters)
  - Identify an array with a length specified separately
  - Identify a location in an array
- This makes it hard to understand what the code does and is supposed to do. It complicates checking and tool support.

```cpp
void use(int* p, int n, char* s, int* q) {
    p[n - 1] = 666; // Bad: we don't know if p points to n elements;
                    // assume it does not or use span<int>
    cout << s; // Bad: we don't know if that s points to a zero-terminated array
               // of char; assume it does not or use zstring
    delete q; // Bad: we don't know if *q is allocated on the free store;
              // assume it does not or use owner
}
```cpp
//better
void use2(span<int> p, zstring s, owner<int*> q) {
    p[p.size() - 1] = 666; // OK, a range error can be caught
    cout << s;             // OK
    delete q;              // OK
}
```
- Note: `owner<T*>` represents ownership, zstring represents a C-style string.
- Also: Assume that a `T*` obtained from a smart pointer to `T` (e.g.,` unique_ptr<T>`) points to a single element.

## F.23: Use a `not_null<T>` to indicate that "null" is not a valid value
- Clarity. A function with a `not_null<T> `parameter makes it clear that the caller of the function is responsible for any nullptr checks that might be necessary.
- Similarly, a function with a return value of `not_null<T>` makes it clear that the caller of the function does not need to check for `nullptr`.
- `not_null<T*>` makes it obvious to a reader (human or machine) that a test for `nullptr` is not necessary before dereference.
- Additionally, when debugging, `owner<T*>` and `not_null<T>` can be instrumented to check for correctness.
- Consider:
```cpp
int length(Record* p);
```
- When I call `length(p)` should I check if `p` is `nullptr` first? Should the implementation of `length()` check if `p` is `nullptr`?
```cpp
// it is the caller's job to make sure p != nullptr
int length(not_null<Record*> p);

// the implementor of length() must assume that p == nullptr is possible
int length(Record* p);
```
- Note: A `not_null<T*>` is assumed not to be the `nullptr`; a `T*` might be the `nullptr`; both can be represented in memory as a `T*` (so no run-time overhead is implied).
- Note: `not_null` is not just for built-in pointers. It works for `unique_ptr`, `shared_ptr`, and other pointer-like types.

## F.24: Use a `span<T>` or a `span_p<T>` to designate a half-open sequence
- Informal/non-explicit ranges are a source of errors.

```cpp
X* find(span<X> r, const X& v); // find v in r

vector<X> vec;
// ...
auto p = find({vec.begin(), vec.end()}, X{}); // find X{} in vec
```
- Note: Ranges are extremely common in C++ code. Typically, they are implicit and their correct use is very hard to ensure.
- In particular, given a pair of arguments `(p, n)` designating an array `[p:p+n)`, it is in general impossible to know if there really are `n` elements to access following `*p`.
- `span<T>` and `span_p<T>` are simple helper classes designating a `[p:q)` range and a range starting with `p` and ending with the first element for which a predicate is true, respectively.
- Example: A `span` represents a range of elements, but how do we manipulate elements of that range?
```cpp
void f(span<int> s) {
    // range traversal (guaranteed correct)
    for (int x : s)
        cout << x << '\n';

    // C-style traversal (potentially checked)
    for (gsl::index i = 0; i < s.size(); ++i)
        cout << s[i] << '\n';

    // random access (potentially checked)
    s[7] = 9;

    // extract pointers (potentially checked)
    std::sort(&s[0], &s[s.size() / 2]);
}****
```
- Note: A `span<T>` object does not own its elements and is so small that it can be passed by value.
- Passing a span object as an argument is exactly as efficient as passing a pair of pointer arguments or passing a pointer and an integer count.

## F.25: Use a `zstring` or a `not_null<zstring>` to designate a C-style string
- C-style strings are ubiquitous. They are defined by convention: **zero-terminated arrays of characters**.
- We must distinguish C-style strings from a pointer to a single character or an old-fashioned pointer to an array of characters.
- If you don't need null termination, use `string_view`.
```cpp
int length(const char* p);
```
- When I call `length(s)` should I check if `s` is `nullptr` first? Should the implementation of `length()` check if `p` is nullptr?
```cpp
// the implementor of length() must assume that p == nullptr is possible
int length(zstring p);

// it is the caller's job to make sure p != nullptr
int length(not_null<zstring> p);
```
- Note: zstring does not represent ownership.

## F.26: Use a `unique_ptr<T>` to transfer ownership where a pointer is needed
- Using `unique_ptr` is the cheapest way to pass a pointer safely.
- See also: [C.50](C.ctor.md#c50-use-a-factory-function-if-you-need-virtual-behavior-during-initialization) regarding when to return a shared_ptr from a factory.
```cpp
unique_ptr<Shape> get_shape(istream& is) // assemble shape from input stream
{
    auto kind =
        read_header(is); // read header and identify the next shape on input
    switch (kind) {
    case kCircle:
        return make_unique<Circle>(is);
    case kTriangle:
        return make_unique<Triangle>(is);
        // ...
    }
}
```
- Note: You need to pass a pointer rather than an object if what you are transferring is an object from a class hierarchy that is to be used through an interface (base class).

## F.27: Use a `shared_ptr<T>` to share ownership
- Using `std::shared_ptr `is the standard way to represent shared ownership. That is, the last owner deletes the object.

```cpp
shared_ptr<const Image> im { read_image(somewhere) };

std::thread t0 {shade, args0, top_left, im};
std::thread t1 {shade, args1, top_right, im};
std::thread t2 {shade, args2, bottom_left, im};
std::thread t3 {shade, args3, bottom_right, im};

// detach threads
// last thread to finish deletes the image
```
- Note: Prefer a `unique_ptr` over a `shared_ptr` if there is never more than one owner at a time. `shared_ptr` is for shared ownership.
- Note that pervasive use of `shared_ptr` has a cost (atomic operations on the shared_ptr's reference count have a measurable aggregate cost).
- Alternative: Have a single object own the shared object (e.g. a scoped object) and destroy that (preferably implicitly) when all users have completed.
