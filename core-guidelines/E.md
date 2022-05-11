# E: Error handling
- Error handling involves:
  - Detecting an error
  - Transmitting information about an error to some handler code
  - Preserving a valid state of the program
  - Avoiding resource leaks
- It is not possible to recover from all errors. If recovery from an error is not possible, it is **important to quickly "get out" in a well-defined way.**
- A strategy for **error handling must be simple**, or it becomes a source of even worse errors.
- **Untested and rarely executed error-handling code is itself the source of many bugs**.

- The rules are designed to help avoid several kinds of errors:
  - Type violations (e.g., misuse of unions and casts)
  - Resource leaks (including memory leaks)
  - Bounds errors
  - Lifetime errors (e.g., accessing an object after is has been deleted)
  - Complexity errors (logical errors made likely by overly complex expression of ideas)
  - Interface errors (e.g., an unexpected value is passed through an interface)


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

