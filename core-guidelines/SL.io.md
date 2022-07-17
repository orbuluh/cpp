# SL.io: Iostream
- iostreams is a type safe, extensible, **formatted and unformatted** I/O library for **streaming I/O**.
- It supports multiple (and user extensible) **buffering strategies and multiple locales.**
- It can be used for conventional I/O, reading and writing to memory (string streams), and user-defined extensions, such as streaming across networks (`asio`: not yet standardized).

- [SL.io: Iostream](#slio-iostream)
  - [SL.io.1: Use character-level input only when you have to](#slio1-use-character-level-input-only-when-you-have-to)
  - [SL.io.2: When reading, always consider ill-formed input](#slio2-when-reading-always-consider-ill-formed-input)
  - [SL.io.3: Prefer iostreams for I/O](#slio3-prefer-iostreams-for-io)
  - [SL.io.10: Unless you use printf-family functions call `ios_base::sync_with_stdio(false)`](#slio10-unless-you-use-printf-family-functions-call-ios_basesync_with_stdiofalse)
  - [SL.io.50: Avoid `std::endl`](#slio50-avoid-stdendl)

## SL.io.1: Use character-level input only when you have to
- Unless you genuinely just deal with individual characters, using character-level input leads to the user code performing potentially error-prone and potentially inefficient composition of tokens out of characters.

```cpp
char c;
char buf[128];
int i = 0;
while (cin.get(c) && !isspace(c) && i < 128)
    buf[i++] = c;
if (i == 128) {
    // ... handle too long string ....
}
```
- Better (much simpler and probably faster):
```cpp
string s;
s.reserve(128);
cin >> s;
```
- and the `reserve(128)` is probably not worthwhile.

## SL.io.2: When reading, always consider ill-formed input
- Errors are typically best handled as soon as possible.
- If input isn't validated, every function must be written to cope with bad data (and that is not practical).


## SL.io.3: Prefer iostreams for I/O
- iostreams are safe, flexible, and extensible.

```cpp
// Example
// write a complex number:
complex<double> z{ 3, 4 };
cout << z << '\n';
```
- complex is a user-defined type and its I/O is defined without modifying the iostream library.
```cpp
// Example
// read a file of complex numbers:
for (complex<double> z; cin >> z; )
    v.push_back(z);
```

- Discussion: iostreams vs. the printf() family
  - It is often (and often correctly) pointed out that the printf() family has two advantages compared to iostreams: flexibility of formatting and performance.
  - This has to be weighed against iostreams advantages of **extensibility to handle user-defined types, resilience against security violations, implicit memory management, and locale handling.**
  - If you need I/O performance, you can almost always do better than `printf()`. `gets()`, `scanf()` using `%s`, and `printf()` using `%s` are security hazards (vulnerable to buffer overflow and generally error-prone).
  - C11 defines some "optional extensions" that do extra checking of their arguments. If present in your C library, `gets_s()`, `scanf_s()`, and `printf_s()` might be safer alternatives, but they are still not type safe.


## SL.io.10: Unless you use printf-family functions call `ios_base::sync_with_stdio(false)`
- Synchronizing iostreams with printf-style I/O can be costly. `cin` and `cout` are by default synchronized with `printf`.

```cpp
int main() {
    ios_base::sync_with_stdio(false);
    // ... use iostreams ...
}
```

## SL.io.50: Avoid `std::endl`
- The `std::endl` manipulator is mostly equivalent to `'\n'` and `"\n";` as most commonly used it **simply slows down output by doing redundant `flush()`s**.
- This slowdown can be significant compared to printf-style output.
- Note: For `cin`/`cout` (and equivalent) interaction, **there is no reason to flush; that's done automatically.**
  - For writing to a file, there is rarely a need to `flush`.
- Note: For string streams (specifically `ostringstream`), the insertion of an `endl` is entirely equivalent to the insertion of a `'\n'` character, but also in this case, `endl` might be significantly slower.
- endl does **not** take care of producing a platform specific end-of-line sequence (like `"\r\n"` on Windows). So for a string stream, `s << endl` just inserts a single character, `'\n'`.
- Note: Apart from the (occasionally important) issue of performance, the choice between `'\n'` and `endl` is almost completely aesthetic.