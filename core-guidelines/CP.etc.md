# CP.etc: Etc. concurrency rules

- [CP.etc: Etc. concurrency rules](#cpetc-etc-concurrency-rules)
  - [CP.200: Use `volatile` only to talk to non-C++ memory](#cp200-use-volatile-only-to-talk-to-non-c-memory)
  - [CP.201: ??? Signals](#cp201--signals)

## CP.200: Use `volatile` only to talk to non-C++ memory
- `volatile` is used to refer to objects that are shared with **"non-C++" code or hardware that does not follow the C++ memory model.**

```cpp
const volatile long clock;
```
- This describes a register constantly updated by a clock circuit. `clock` is `volatile` because its value will change without any action from the C++ program that uses it.
- For example, reading `clock` twice will often yield two different values, so the optimizer had better not optimize away the second read in this code:
```cpp
long t1 = clock;
// ... no use of clock here ...
long t2 = clock;
```
- clock is `const` because the **program should not try to write to `clock`.**
- Note: Unless you are writing the lowest level code manipulating hardware directly, consider `volatile` an esoteric feature that is **best avoided**.

- Example, usually C++ code receives `volatile` memory that is owned elsewhere (hardware or another language):

```cpp
int volatile* vi = get_hardware_memory_location();
    // note: we get a pointer to someone else's memory here
    // volatile says "treat this with extra respect"
```
- Sometimes C++ code allocates the `volatile` memory and shares it with "elsewhere" (hardware or another language) by **deliberately escaping a pointer**:

```cpp
static volatile long vl;
please_use_this(&vl);   // escape a reference to this to "elsewhere" (not C++)
```

- **`volatile` local variables are nearly always wrong** -- how can they be shared with other languages or hardware if they're ephemeral?
  - The same applies almost as strongly to **member variables**, for the same reason.

```cpp
void f() {
    volatile int i = 0; // bad, volatile local variable
    // etc.
}

class My_type {
    volatile int i = 0; // suspicious, volatile member variable
    // etc.
};
```
- Note: In C++, unlike in some other languages, **`volatile` has nothing to do with synchronization.**

## CP.201: ??? Signals
- UNIX signal handling .... Might be worth reminding **how little is async-signal-safe, and how to communicate with a signal handler (best is probably "not at all")**