# address operations

[Code example](address_ops.h)


[Discussion: Getting the difference between two memory addresses](https://stackoverflow.com/questions/12849588/getting-the-difference-between-two-memory-addresses)

- Note: Pointer difference is of type `ptrdiff_t`. Casting it to int may get one into trouble.
- Check your code sample. But overall, for `T* a` and `T* b`, `(b - a)` equal to number of `T` in between `b` and `a`. So if `a` and `b` are next to each other, the result will be 1, not `sizeof(T)` (bytes)!
