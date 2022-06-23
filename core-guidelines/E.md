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

