# R.alloc: Allocation and deallocation

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
