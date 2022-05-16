# I: Interfaces
- An interface is a **contract between two parts of a program**.
  - Precisely stating what is expected of a supplier of a service and a user of that service is essential.
  - Having good (easy-to-understand, encouraging efficient use, not error-prone, supporting testing, etc.) **interfaces is probably the most important single aspect of code organization.**



## I.3: Avoid singletons
- Singletons are basically complicated global objects in disguise.
- If you don't want a global object to change, declare it `const` or `constexpr`.
- Exception: You can use the simplest "singleton" to get initialization on first use, if any, e.g.
```cpp
X& myX()
{
    static X my_x {3};
    return my_x;
}
```
- This is one of the most effective solutions to problems related to initialization order. In a multi-threaded environment, the initialization of the static object does not introduce a race condition (unless you carelessly access a shared object from within its constructor).
- Note that the initialization of a local static does not imply a race condition. However, if the destruction of X involves an operation that needs to be synchronized we must use a less simple solution.
```cpp
X& myX()
{
    static auto p = new X {3};
    return *p;  // potential leak
}
```
- Now someone must delete that object in some suitably thread-safe way. That's error-prone, so we don't use that technique unless
  - `myX` is in multi-threaded code,
  - that `X` object needs to be destroyed (e.g., because it releases a resource), and
  - `X`'s destructor's code needs to be synchronized.
- If you, as many do, define a singleton as a class for which only one object is created, functions like `myX` are not singletons, and this useful technique is not an exception to the no-singleton rule.


## I.25: Prefer empty abstract classes as interfaces to class hierarchies
- not yet read
## I.27: For stable library ABI, consider the Pimpl idiom
- not yet read