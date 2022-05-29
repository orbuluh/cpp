# ES: Expressions and statements
- Expressions and statements are the lowest and most direct way of expressing actions and computation. Declarations in local scopes are statements.

## ES.1: Prefer the standard library to other libraries and to "handcrafted code"
- Code using a library can be much easier to write than code working directly with language features, much shorter, tend to be of a higher level of abstraction, and the library code is presumably already tested. The ISO C++ Standard Library is among the most widely known and best tested libraries. It is available as part of all C++ implementations.
- Large parts of the standard library rely on dynamic allocation (free store). These parts, notably the containers but not the algorithms, are unsuitable for some hard-real-time and embedded applications. In such cases, consider providing/using similar facilities, e.g., a standard-library-style container implemented using a pool allocator.

## ES.2: Prefer suitable abstractions to direct use of language features
- A "suitable abstraction" (e.g., library or class) is **closer to the application concepts than the bare language**, leads to shorter and clearer code, and is likely to be better tested.

```cpp
vector<string> read1(istream& is)   // good
{
    vector<string> res;
    for (string s; is >> s;)
        res.push_back(s);
    return res;
}
```
- The more traditional and lower-level near-equivalent is longer, messier, harder to get right, and most likely slower:
```cpp
char** read2(istream& is, int maxelem, int maxstring,
             int* nread) // bad: verbose and incomplete
{
    auto res = new char*[maxelem];
    int elemcount = 0;
    while (is && elemcount < maxelem) {
        auto s = new char[maxstring];
        is.read(s, maxstring);
        res[elemcount++] = s;
        // Once the checking for overflow and error handling
        // has been added that code gets quite messy, and there
        // is the problem remembering to delete the returned
        // pointer and the C-style strings that array contains....
    }
    nread = &elemcount;
    return res;
}
```

## ES.3: Don't repeat yourself, avoid redundant code
- Duplicated or otherwise redundant code obscures intent, makes it harder to understand the logic, and makes maintenance harder, among other problems. It often arises from cut-and-paste programming.
- Use standard algorithms where appropriate, instead of writing some own implementation.
- See also: [SL.1](SL.md#sl1-use-libraries-wherever-possible), [ES.11](ES.dcl.md#es11-use-auto-to-avoid-redundant-repetition-of-type-names)

# Subsections
- [ES.dcl: Declarations](ES.dcl.md)