# E: Error handling
- Error handling involves:
  - Detecting an error
  - Transmitting information about an error to some handler code
  - Preserving a valid state of the program
  - Avoiding resource leaks
- It is not possible to recover from all errors.
- If recovery from an error is not possible, it is **important to quickly "get out" in a well-defined way.**
  - A strategy for **error handling must be simple**, or it becomes a source of even worse errors.
  - **Untested and rarely executed error-handling code is itself the source of many bugs**.

- The rules are designed to help avoid several kinds of errors:
  - Type violations (e.g., misuse of unions and casts)
  - Resource leaks (including memory leaks)
  - Bounds errors
  - Lifetime errors (e.g., accessing an object after is has been deleted)
  - Complexity errors (logical errors made likely by overly complex expression of ideas)
  - Interface errors (e.g., an unexpected value is passed through an interface)


## E.1: Develop an error-handling strategy early in a designReason
- A consistent and complete strategy for handling errors and resource leaks is hard to retrofit into a system.

## E.2: Throw an exception to signal that a function can't perform its assigned task
- To make error handling systematic, robust, and non-repetitive.

```cpp
struct Foo {
    vector<Thing> v;
    File_handle f;
    string s;
};

void use() {
    Foo bar{
        {Thing{1}, Thing{2}, Thing{monkey}}, {"my_file", "r"}, "Here we go!"};
    // ...
}
```
- Here, `vector` and `strings` constructors might not be able to allocate sufficient memory for their elements, vectors constructor might not be able copy the `Things` in its initializer list, and `File_handle` might not be able to open the required file.
- In each case, they throw an exception for use()'s caller to handle.
- If `use()` could handle the failure to construct `bar` it can take control using try/catch.
- In either case, `Foo`'s constructor correctly destroys constructed members before passing control to whatever tried to create a `Foo`.
- Note that there is no return value that could contain an error code.

- The `File_handle` constructor might be defined like this:
```cpp
File_handle::File_handle(const string& name, const string& mode)
    : f{fopen(name.c_str(), mode.c_str())} {
    if (!f)
        throw runtime_error{"File_handle: could not open " + name + " as " + mode};
}
```
- Note: It is often said that exceptions are meant to signal exceptional events and failures.
  - However, that's a bit circular because "what is exceptional?"
- Examples: **A precondition that cannot be met**
  - A constructor that cannot construct an object (failure to establish its class's invariant)
  - An out-of-range error (e.g., `v[v.size()] = 7`)
  - Inability to acquire a resource (e.g., the network is down)
- In contrast, termination of an ordinary loop is not exceptional. Unless the loop was meant to be infinite, termination is normal and expected.
- Note: Don't use a throw as simply an alternative way of returning a value from a function.
- Exception: Some systems, such as hard-real-time systems require a guarantee that an action is taken in a (typically short) constant maximum time known before execution starts.
  - Such systems c**an use exceptions only if there is tool support for accurately predicting the maximum time to recover from a throw**.
- Note: Before deciding that you cannot afford or don't like exception-based error handling, have a look at the alternatives; they have their own complexities and problems.
  - Also, **as far as possible, measure before making claims about efficiency.**

## E.3: Use exceptions for error handling only
- To keep error handling separated from "ordinary code."
- C++ implementations tend to be **optimized based on the assumption that exceptions are rare.**

```cpp
// don't: exception not used for error handling
int find_index(vector<string>& vec, const string& x) {
    try {
        for (gsl::index i = 0; i < vec.size(); ++i)
            if (vec[i] == x)
                throw i; // found x
    } catch (int i) {
        return i;
    }
    return -1; // not found
}
```
- This is more complicated and most likely runs much slower than the obvious alternative.
- There is nothing exceptional about finding a value in a vector.


## E.4: Design your error-handling strategy around invariants
- To use an object it must be in a valid state (defined formally or informally by an **invariant**) and to recover from an error every object not destroyed must be in a valid state.
- An invariant is a **logical condition** for the members of an object that a constructor must establish for the public member functions to assume.

## E.5: Let a constructor establish an invariant, and throw if it cannot
- Leaving an object without its invariant established is asking for trouble.
- Not all member functions can be called.

```cpp
class Vector { // very simplified vector of doubles
    // if elem != nullptr then elem points to sz doubles
  public:
    Vector() : elem{nullptr}, sz{0} {}
    Vector(int s) : elem{new double[s]}, sz{s} { /* initialize elements */
    }
    ~Vector() { delete[] elem; }
    double& operator[](int s) { return elem[s]; }
    // ...
  private:
    owner<double*> elem;
    int sz;
};
```
- The class invariant - here stated as a comment - is established by the constructors.
- `new` throws if it cannot allocate the required memory.
- The operators, notably the subscript operator, relies on the invariant.


## E.6: Use RAII to prevent leaks
- Leaks are typically unacceptable. Manual resource release is error-prone.
- RAII ("Resource Acquisition Is Initialization") is the simplest, most systematic way of preventing leaks.

```cpp
void f1(int i) // Bad: possible leak
{
    int* p = new int[12];
    // ...
    if (i < 17)
        throw Bad{"in f()", i};
    // ...
}
```
- We could carefully release the resource before the throw:

```cpp
void f3(int i) // OK: resource management done by a handle (but see below)
{
    auto p = make_unique<int[]>(12);
    // ...
    if (i < 17)
        throw Bad{"in f()", i};
    // ...
}
```
- Note that this works even when the throw is implicit because it happened in a called function:
```cpp
void f4(int i) // OK: resource management done by a handle (but see below)
{
    auto p = make_unique<int[]>(12);
    // ...
    helper(i); // might throw
    // ...
}
```
- Unless you really need pointer semantics, use a local resource object. That's even simpler and safer, and often more efficient.

```cpp
void f5(int i) // OK: resource management done by local object
{
    vector<int> v(12);
    // ...
    helper(i); // might throw
    // ...
}
```
- Note: If there is no obvious resource handle and for some reason defining a proper RAII object/handle is infeasible, as a last resort, **cleanup actions can be represented by a final_action object.**
- Note: But what do we do if we are writing a program where exceptions cannot be used?
  - **First challenge that assumption;** there are many anti-exceptions myths around.
- We know of only a few good reasons:
  - We are on a system so small that the exception support would eat up most of our 2K memory.
  - We are in a hard-real-time system and we don't have tools that guarantee us that an exception is handled within the required time.
  - We are in a system with tons of legacy code using lots of pointers in difficult-to-understand ways (in particular without a recognizable ownership strategy) so that exceptions could cause leaks.
  - Our implementation of the C++ exception mechanisms is unreasonably poor (slow, memory consuming, failing to work correctly for dynamically linked libraries, etc.). Complain to your implementation purveyor; if no user complains, no improvement will happen.
  - We get fired if we challenge our manager's ancient wisdom.
- Only the first of these reasons is fundamental, so whenever possible, use exceptions to implement RAII, or design your RAII objects to never fail.
- When exceptions cannot be used, simulate RAII. That is, systematically check that objects are valid after construction and still release all resources in the destructor.
- One strategy is to add a `valid()` operation to every resource handle:

```cpp
void f() {
    vector<string> vs(100); // not std::vector: valid() added
    if (!vs.valid()) {
        // handle error or exit
    }

    ifstream fs("foo"); // not std::ifstream: valid() added
    if (!fs.valid()) {
        // handle error or exit
    }

    // ...
} // destructors clean up as usual
```
- Obviously, this increases the size of the code, doesn't allow for implicit propagation of "exceptions" (`valid()` checks), and `valid()` checks can be forgotten. **Prefer to use exceptions.**

## E.7: State your preconditions
- To avoid interface errors.

## E.8: State your postconditions
- To avoid interface errors.

## E.12: Use `noexcept` when exiting a function because of a throw is **impossible or unacceptable**
- To make error handling systematic, robust, and efficient.

```cpp
double compute(double d) noexcept { return log(sqrt(d <= 0 ? 1 : d)); }
```
- Here, we know that compute will not throw because it is composed out of operations that don't throw.
- By declaring compute to be `noexcept`, **we give the compiler and human readers information that can make it easier for them to understand and manipulate compute.**
- Note: Many standard-library functions are `noexcept` including all the standard-library functions "inherited" from the C Standard Library.

```cpp
vector<double> munge(const vector<double>& v) noexcept {
    vector<double> v2(v.size());
    // ... do something ...
}
```
- The `noexcept` here states that **I am not willing or able to handle the situation where I cannot construct the local vector.**
  - That is, I consider memory exhaustion a serious design error (on par with hardware failures) so that **I'm willing to crash the program if it happens.**
- Note: Do not use traditional exception-specifications.

## E.13: Never throw while being the direct owner of an object
- That would be a leak.

Example
```cpp
void leak(int x) // don't: might leak
{
    auto p = new int{7};
    if (x < 0)
        throw Get_me_out_of_here{}; // might leak *p
    // ...
    delete p; // we might never get here
}
```
- One way of avoiding such problems is to use resource handles consistently:
```cpp
void no_leak(int x) {
    auto p = make_unique<int>(7);
    if (x < 0)
        throw Get_me_out_of_here{}; // will delete *p if necessary
    // ...
    // no need for delete p
}
```
- Another solution (often better) would be to **use a local variable to eliminate explicit use of pointers:**
```cpp
void no_leak_simplified(int x)
{
    vector<int> v(7);
    // ...
}
```
- Note: If you have a local "thing" that requires cleanup, but is not represented by an object with a destructor, such cleanup must also be done before a throw.
- **Sometimes, `finally()` can make such unsystematic cleanup a bit more manageable.**


## E.14: Use purpose-designed user-defined types as exceptions (not built-in types)
- A user-defined type can better transmit information about an error to a handler.
- Information can be encoded into the type itself and the type is unlikely to clash with other people's exceptions.

```cpp
throw 7; // bad
throw "something bad";  // bad
throw std::exception{}; // bad - no info
```
- Deriving from `std::exception` gives the flexibility to catch the specific exception or handle generally through `std::exception`:
```cpp
class MyException : public std::runtime_error
{
public:
    MyException(const string& msg) : std::runtime_error{msg} {}
    // ...
};
// ...
throw MyException{"something bad"};  // good
```
- Exceptions do not need to be derived from std::exception:
```cpp
class MyCustomError final {};  // not derived from std::exception

// ...

throw MyCustomError{};  // good - handlers must catch this type (or ...)
```
- Library types derived from `std::exceptio`n can be used as generic exceptions if no useful information can be added at the point of detection:
```cpp
throw std::runtime_error("someting bad"); // good

// ...
throw std::invalid_argument("i is not even"); // good
//enum classes are also allowed:
enum class alert {RED, YELLOW, GREEN};
throw alert::RED; // good
```

## E.15: Throw by **value**, catch exceptions from a hierarchy by **reference**
- Throwing by value (not by pointer) and catching by reference prevents copying, especially **slicing** base subobjects.
```cpp
// Example; bad
void f() {
    try {
        // ...
        throw new widget{}; // don't: throw by value not by raw pointer
        // ...
    } catch (base_class e) { // don't: might slice
        // ...
    }
}
```
- Instead, use a reference:
```cpp
  catch (base_class& e) { /* ... */ }
```
- or - typically better still - a const reference:
```cpp
catch (const base_class& e) { /* ... */ }
```
- Most handlers do not modify their exception and in general we recommend use of const.
- Note: Catch by value can be appropriate for a small value type such as an enum value.
- Note: To rethrow a caught exception use throw; not `throw e;`. Using `throw e;` would throw a new copy of `e` (sliced to the static type `std::exception`) instead of rethrowing the original exception of type `std::runtime_error`.
- (But keep **Don't try to catch every exception in every function and Minimize the use of explicit try/catch in mind.**)

## E.16: Destructors, deallocation, and swap must never fail
- **The standard library assumes that destructors, deallocation functions (e.g., operator delete), and swap do not throw.** If they do, basic standard-library invariants are broken.
- We don't know how to write reliable programs if a destructor, a swap, or a memory deallocation fails; that is, if it exits by an exception or simply doesn't perform its required action.
```cpp
class Connection {
  public:
    ~Connection() // Don't: very bad destructor
    {
        if (cannot_disconnect())
            throw I_give_up{information};
        // ...
    }
};
```
- Many have tried to write reliable code violating this rule for examples, such as a network connection that "refuses to close".
- To the best of our knowledge nobody has found a general way of doing this. Occasionally, for very specific examples, you can get away with setting some state for future cleanup.
- For example, we might put a socket that does not want to close on a "bad socket" list, to be examined by a regular sweep of the system state. **Every example we have seen of this is error-prone, specialized, and often buggy.**
- Deallocation functions, including operator delete, must be noexcept. swap functions must be noexcept. Most destructors are implicitly noexcept by default. Also, [make move operations noexcept](C.copy.md#c66-make-move-operations-noexcept).

## E.17: Don't try to catch every exception in every function
- Catching an exception in a function that cannot take a meaningful recovery action leads to complexity and waste.
- Let an exception propagate until it reaches a function that can handle it.
- Let cleanup actions on the unwinding path be handled by RAII.

```cpp
//Example, don't
void f() // bad
{
    try {
        // ...
    } catch (...) {
        // no action
        throw; // propagate exception
    }
}
```

## E.18: Minimize the use of explicit try/catch
- try/catch is verbose and non-trivial uses are error-prone.
- try/catch can be a sign of unsystematic and/or low-level resource management or error handling.

```cpp
//Example, Bad
void f(zstring s) {
    Gadget* p;
    try {
        p = new Gadget(s);
        // ...
        delete p;
    } catch (Gadget_construction_failure) {
        delete p;
        throw;
    }
}
```
- This code is messy.
  - There could be a leak from the naked pointer in the try block.
  - Not all exceptions are handled.
  - Deleting an object that failed to construct is almost certainly a mistake. 
- Better:
```cpp
void f2(zstring s)
{
    Gadget g {s};
}
```
- Alternatives:
  - proper resource handles and RAII
  - finally


## E.19: Use a `final_action` object to express cleanup if no suitable resource handle is available
- `finally` is less verbose and harder to get wrong than try/catch.

```cpp
void f(int n)
{
    void* p = malloc(n);
    auto _ = finally([p] { free(p); });
    // ...
}
```
- Note: `finally` is not as messy as try/catch, but it is still ad-hoc
- Prefer proper resource management objects. Consider `finally` a last resort.
- Note: Use of `finally` is a systematic and reasonably clean alternative to the old `goto exit;` technique for dealing with cleanup where resource management is not systematic.

## E.25: If you can't throw exceptions, simulate RAII for resource management
- Even without exceptions, RAII is usually the best and most systematic way of dealing with resources.

- Note: Error handling using exceptions is the only complete and systematic way of handling **non-local** errors in C++.
- In particular,
  - non-intrusively signaling failure to construct an object requires an exception.
  - Signaling errors in a way that cannot be ignored requires exceptions.
  - If you can't use exceptions, simulate their use as best you can.

- A lot of fear of exceptions is misguided.
  - When used for exceptional circumstances in code that is not littered with pointers and complicated control structures, exception handling is almost always affordable (in time and space) and almost always leads to better code.
  - This, of course, assumes a good implementation of the exception handling mechanisms, which is not available on all systems.
- There are also cases where the problems above do not apply, but exceptions cannot be used for other reasons.
  - Some hard-real-time systems are an example: An operation has to be completed within a fixed time with an error or a correct answer.
  - In the absence of appropriate time estimation tools, this is hard to guarantee for exceptions.
  - Such systems (e.g. flight control software) typically also ban the use of dynamic (heap) memory.

- So, the primary guideline for error handling is "use exceptions and RAII."
  - This section deals with the cases where you either do not have an efficient implementation of exceptions, or have such a rat's nest of old-style code (e.g., lots of pointers, ill-defined ownership, and lots of unsystematic error handling based on tests of error codes) that it is infeasible to introduce simple and systematic exception handling.
- Before condemning exceptions or complaining too much about their cost,
  - consider examples of the use of error codes.
  - Consider the cost and complexity of the use of error codes.
  - If performance is your worry, measure.

- Example: Assume you wanted to write
```cpp
void func(zstring arg)
{
    Gadget g {arg};
    // ...
}
```
- If the gadget isn't correctly constructed, `func` exits with an exception.
- If we cannot throw an exception, we can simulate this RAII style of resource handling by adding a `valid(`) member function to `Gadget`:
```cpp
error_indicator func(zstring arg) {
    Gadget g{arg};
    if (!g.valid())
        return gadget_construction_error;
    // ...
    return 0; // zero indicates "good"
}
```
- The problem is of course that the caller now has to remember to test the return value. To encourage doing so, consider adding a `[[nodiscard]]`.