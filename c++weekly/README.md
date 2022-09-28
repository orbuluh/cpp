# C++ weekly notes

# [Ep 322 - Top 4 Places To Never Use `const`](https://youtu.be/dGCxMmGvocE)
- [code snippet](demo/no_const.h)
- don't `const` non-reference return types (it breaks copy elision when return)
- don't `const` local values that need take advantage of implicit move-on-return operations
```cpp
inline std::optional<S> make_value_3() {
  const S s; // bad because this object and the return type
             // are different! The optional will make a copy from the local s
  return s;
}
```
- Note: if you have multiple different objects that might be returned, then you are also relying on implicit move-on-return (aka automatic move)
```cpp
inline S make_value_3_1(bool option) {
  if (option) {
    const S s; // bad use of const
    return s;
  } else {
    const S s2; // bad use of const
    return s2; // better would be to return S{} directly!
  }
}
```
- don't `const` non-trivial value parameters that you might need to return directly from the function
```cpp
inline S make_value_2(const S s) // return statement makes this bad use of `const`
{
  return s; // because we return it, const is bad in function definition!
}
```
- don't `const` *any* member data!
  - it breaks implicit and explicit moves
  - it breaks common use cases of assignment