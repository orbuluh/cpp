# R.alloc: Allocation and deallocation

- [R.alloc: Allocation and deallocation](#ralloc-allocation-and-deallocation)
  - [R.10: Avoid `malloc()` and `free()`](#r10-avoid-malloc-and-free)
  - [R.11: Avoid calling `new` and `delete` explicitly](#r11-avoid-calling-new-and-delete-explicitly)
  - [R.12: Immediately give the result of an explicit resource allocation to a manager object](#r12-immediately-give-the-result-of-an-explicit-resource-allocation-to-a-manager-object)
  - [R.13: Perform at most one explicit resource allocation in a single expression statement](#r13-perform-at-most-one-explicit-resource-allocation-in-a-single-expression-statement)
  - [R.14: Avoid `[]` parameters, prefer `span`](#r14-avoid--parameters-prefer-span)
  - [R.15: Always overload matched allocation/deallocation pairs](#r15-always-overload-matched-allocationdeallocation-pairs)

## R.10: Avoid `malloc()` and `free()`
- `malloc()` and `free()` do not support construction and destruction, and do not mix well with `new` and `delete`.
- In some implementations that delete and that free() might work, or maybe they will cause run-time errors.

## R.11: Avoid calling `new` and `delete` explicitly
- The pointer returned by `new` should belong to a resource handle (that can call `delete`).
- If the pointer returned by `new` is assigned to a plain/naked pointer, the object can be leaked.
- In a large program, a naked delete (that is a delete in application code, rather than part of code devoted to resource management) is a likely bug: if you have N deletes, how can you be certain that you don't need N+1 or N-1?
  - The bug might be latent: it might emerge only during maintenance. If you have a naked new, you probably need a naked delete somewhere, so you probably have a bug.

## R.12: Immediately give the result of an explicit resource allocation to a manager object
- If you don't, an exception or a return might lead to a leak.
```cpp
void f(const string& name) {
    FILE* f = fopen(name, "r"); // open the file
    vector<char> buf(1024); // The allocation of buf might fail and leak the file handle.
    auto _ = finally([f] { fclose(f); }); // remember to close the file ... but what if above throw already...
    // ...
}

// instead...
void f(const string& name) {
    ifstream f{name}; // open the file with RAII already
    vector<char> buf(1024);
    // ...
}
```

## R.13: Perform at most one explicit resource allocation in a single expression statement
- If you perform two explicit resource allocations in one statement, you could leak resources because **the order of evaluation of many subexpressions, including function arguments, is unspecified**.
```cpp
void fun(shared_ptr<Widget> sp1, shared_ptr<Widget> sp2);

// someone could have done this ...

fun(shared_ptr<Widget>(new Widget(a, b)), shared_ptr<Widget>(new Widget(c, d))); // BAD: potential leak
```
- This is exception-unsafe because the compiler might reorder the two expressions building the function's two arguments.
- In particular, the compiler can interleave execution of the two expressions: Memory allocation (by calling operator new) could be done first for both objects, followed by attempts to call the two Widget constructors.
- If one of the constructor calls throws an exception, then the other object's memory will never be released!
- This subtle problem has a simple solution: Never perform more than one explicit resource allocation in a single expression statement. 
- For example:
```cpp
shared_ptr<Widget> sp1(new Widget(a, b)); // Better, but messy
fun(sp1, new Widget(c, d));
```
- The best solution is to **avoid explicit allocation entirely use factory functions that return owning objects**:
```cpp
fun(make_shared<Widget>(a, b), make_shared<Widget>(c, d)); // Best
```
- Write your own factory wrapper if there is not one already.

## R.14: Avoid `[]` parameters, prefer `span`
- An array decays to a pointer, thereby losing its size, opening the opportunity for range errors.
- Use `span` to preserve size information.
```cpp
void f(int[]);          // not recommended

void f(int*);           // not recommended for multiple objects
                        // (a pointer should point to a single object, do not subscript)

void f(gsl::span<int>); // good, recommended
```

## R.15: Always overload matched allocation/deallocation pairs
- Otherwise you get mismatched operations and chaos...
```cpp
class X {
    // ...
    void* operator new(size_t s);
    void operator delete(void*);
    // ...
};
```
- If you want memory that cannot be deallocated, `=delete` the deallocation operation. **Don't leave it undeclared.**

