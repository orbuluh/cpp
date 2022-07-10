# SL.str: String

- Text manipulation is a huge topic. `std::string` doesn't cover all of it.
- This section primarily tries to clarify `std::string`'s relation to `char*`, `zstring`, `string_view`, and `gsl::span<char>`.
- The important issue of non-ASCII character sets and encodings (e.g., `wchar_t`, Unicode, and UTF-8) will be covered elsewhere.

- See also: [regular expressions](SL.md#slregex-regex)
- Here, we use "sequence of characters" or "string" to refer to a sequence of characters meant to be read as text (somehow, eventually).


## SL.str.1: Use `std::string` to own character sequences
- string correctly handles allocation, ownership, copying, gradual expansion, and offers a variety of useful operations.

```cpp
vector<string> read_until(const string& terminator) {
    vector<string> res;
    for (string s; cin >> s && s != terminator;) // read a word
        res.push_back(s);
    return res;
}
```
- Note how `>>` and `!=` are provided for `string` (as examples of useful operations) and there are no explicit allocations, deallocations, or range checks (string takes care of those).
- In C++17, we might use `string_view` as the argument, rather than `const string&` to allow more flexibility to callers:

```cpp
vector<string> read_until(string_view terminator) // C++17
{
    vector<string> res;
    for (string s; cin >> s && s != terminator;) // read a word
        res.push_back(s);
    return res;
}
```
- Don't use C-style strings for operations that require non-trivial memory management

```cpp
char* cat(const char* s1, const char* s2) // beware!
                                          // return s1 + '.' + s2
{
    int l1 = strlen(s1);
    int l2 = strlen(s2);
    char* p = (char*)malloc(l1 + l2 + 2);
    strcpy(p, s1, l1);
    p[l1] = '.';
    strcpy(p + l1 + 1, s2, l2);
    p[l1 + l2 + 1] = 0;
    return p;
}
```
- Did we get that right? Will the caller remember to `free()` the returned pointer? Will this code pass a security review?
- Note: **Do not assume that string is slower than lower-level techniques without measurement and remember that not all code is performance critical.** Don't optimize prematurely.

## SL.str.2: Use `std::string_view` or `gsl::span<char>` to refer to character sequences
- `std::string_view` or `gsl::span<char>` provides simple and (potentially) safe access to character sequences independently of how those sequences are allocated and stored.

```cpp
vector<string> read_until(string_view terminator);

void user(zstring p, const string& s, string_view ss)
{
    auto v1 = read_until(p);
    auto v2 = read_until(s);
    auto v3 = read_until(ss);
    // ...
}
```
- Note: `std::string_view` (C++17) is read-only.

## SL.str.3: Use `zstring` or `czstring` to refer to a C-style, zero-terminated, sequence of characters
- Readability. Statement of intent.
- A plain `char*` can be a pointer to a single character, a pointer to an array of characters, a pointer to a C-style (zero-terminated) string, or even to a small integer.
- Distinguishing these alternatives prevents misunderstandings and bugs.

```cpp
void f1(const char* s); // s is probably a string
```
- All we know is that it is supposed to be the `nullptr` or point to at least one character

```cpp
void f1(zstring s);     // s is a C-style string or the nullptr
void f1(czstring s);    // s is a C-style string constant or the nullptr
void f1(std::byte* s);  // s is a pointer to a byte (C++17)
```
- Note: Don't convert a C-style string to string unless there is a reason to.
- Note: Like any other "plain pointer", a zstring should not represent ownership.
- Note: There are billions of lines of C++ "out there", most use `char*` and `const char*` without documenting intent.
  - They are used in a wide variety of ways, including to represent ownership and as generic pointers to memory (instead of void*).
  - It is hard to separate these uses, so this guideline is hard to follow.
  - This is one of the major sources of bugs in C and C++ programs, so it is worthwhile to follow this guideline wherever feasible.


## SL.str.4: Use `char*` to refer to a single character
- The variety of uses of char* in current code is a major source of errors.

```cpp
//Example, bad
char arr[] = {'a', 'b', 'c'};

void print(const char* p) { cout << p << '\n'; }

void use() {
    print(arr); // run-time error; potentially very bad
}
```
- The array `arr` is not a C-style string because it is not zero-terminated.

## SL.str.5: Use `std::byte` to refer to byte values that do not necessarily represent characters
- Use of `char*` to represent a pointer to something that is not necessarily a character causes confusion and disables valuable optimizations.

## SL.str.10: Use `std::string` when you need to perform locale-sensitive string operations
- `std::string` supports standard-library locale facilities

## SL.str.11: Use `gsl::span<char>` rather than `std::string_view` when you need to mutate a string
- `std::string_view` is read-only.

## SL.str.12: Use the s suffix for string literals meant to be standard-library strings
- Direct expression of an idea minimizes mistakes.

```cpp
auto pp1 = make_pair("Tokyo", 9.00);        // {C-style string,double} intended?
pair<string, double> pp2 = {"Tokyo", 9.00}; // a bit verbose

auto pp3 = make_pair("Tokyo"s, 9.00);       // {std::string,double}    // C++14
pair pp4 = {"Tokyo"s, 9.00};                // {std::string,double}    // C++17
```