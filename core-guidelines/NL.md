# NL: Naming and layout rules
- Consistent naming and layout are helpful.
- If for no other reason because it minimizes "my style is better than your style" arguments.
- However, there are many, many, different styles around and people are passionate about them (pro and con).
- Also, most real-world projects include code from many sources, so **standardizing on a single style for all code is often impossible.*
- After many requests for guidance from users, we present a set of rules that you might use if you have no better ideas, but the **real aim is consistency, rather than any particular rule set**.
- IDEs and tools can help (as well as hinder).
- Most of these rules are aesthetic and programmers hold strong opinions. IDEs also tend to have defaults and a range of alternatives. These rules are suggested defaults to follow unless you have reasons not to.
- We have had comments to the effect that naming and layout are so personal and/or arbitrary that **we should not try to "legislate" them.** We are not "legislating" (see the previous paragraph). However, we have had many requests for a set of naming and layout conventions to use when there are no external constraints.
  - More specific and detailed rules are easier to enforce.
  - These rules bear a strong resemblance to the recommendations in the PPP Style Guide written in support of Stroustrup's Programming: Principles and Practice using C++.

- [NL: Naming and layout rules](#nl-naming-and-layout-rules)
  - [NL.1: Don't say in comments what can be clearly stated in code](#nl1-dont-say-in-comments-what-can-be-clearly-stated-in-code)
  - [NL.2: State intent in comments](#nl2-state-intent-in-comments)
  - [NL.3: Keep comments crisp](#nl3-keep-comments-crisp)
  - [NL.4: Maintain a consistent indentation style](#nl4-maintain-a-consistent-indentation-style)
  - [NL.5: Avoid encoding type information in names](#nl5-avoid-encoding-type-information-in-names)
  - [NL.7: Make the length of a name roughly proportional to the length of its scope](#nl7-make-the-length-of-a-name-roughly-proportional-to-the-length-of-its-scope)
  - [NL.8: Use a consistent naming style](#nl8-use-a-consistent-naming-style)
  - [NL.9: Use ALL_CAPS for macro names only](#nl9-use-all_caps-for-macro-names-only)
  - [NL.10: Prefer underscore_style names](#nl10-prefer-underscore_style-names)
  - [NL.11: Make literals readable](#nl11-make-literals-readable)
  - [NL.15: Use spaces sparingly](#nl15-use-spaces-sparingly)
  - [NL.16: Use a conventional class member declaration order](#nl16-use-a-conventional-class-member-declaration-order)
  - [NL.17: Use K&R-derived layout](#nl17-use-kr-derived-layout)
  - [NL.18: Use C++-style declarator layout](#nl18-use-c-style-declarator-layout)
  - [NL.19: Avoid names that are easily misread](#nl19-avoid-names-that-are-easily-misread)
  - [NL.20: Don't place two statements on the same line](#nl20-dont-place-two-statements-on-the-same-line)
  - [NL.21: Declare one name (only) per declaration](#nl21-declare-one-name-only-per-declaration)
  - [NL.25: Don't use `void` as an argument type](#nl25-dont-use-void-as-an-argument-type)
  - [NL.26: Use conventional `const` notation](#nl26-use-conventional-const-notation)

## NL.1: Don't say in comments what can be clearly stated in code
- Compilers do not read comments. Comments are less precise than code. Comments are not updated as consistently as code.

## NL.2: State intent in comments
- **Code says what is done, not what is supposed to be done.**
- Often **intent** can be stated more clearly and concisely than the implementation.

```cpp
void stable_sort(Sortable& c)
    // sort c in the order determined by <, keep equal elements (as defined by ==) in
    // their original relative order
{
    // ... quite a few lines of non-trivial code ...
}
```
- Note: If the comment and the code disagree, both are likely to be wrong.

## NL.3: Keep comments crisp
- Verbosity slows down understanding and makes the code harder to read by spreading it around in the source file.
- Note: Use intelligible English. I might be fluent in Danish, but most programmers are not; the maintainers of my code might not be. Avoid SMS lingo and watch your grammar, punctuation, and capitalization. Aim for professionalism, not "cool."

## NL.4: Maintain a consistent indentation style
- Readability. Avoidance of "silly mistakes."
- Note:  Always indenting the statement after if (...), for (...), and while (...) is usually a good idea:

## NL.5: Avoid encoding type information in names
- Rationale: If names reflect types rather than functionality, it becomes hard to change the types used to provide that functionality.
- Also, if the type of a variable is changed, code using it will have to be modified.
- Minimize unintentional conversions.

```cpp
void print_int(int i);
void print_string(const char*);

print_int(1);          // repetitive, manual type matching
print_string("xyzzy"); // repetitive, manual type matching
//Example, good
void print(int i);
void print(string_view);    // also works on any string-like sequence

print(1);              // clear, automatic type matching
print("xyzzy");        // clear, automatic type matching
```
- Note: Names with types encoded are either verbose or cryptic.
```cpp
printS  // print a std::string
prints  // print a C-style string
printi  // print an int
```
- Requiring techniques like Hungarian notation to encode a type has been used in untyped languages, but is generally unnecessary and actively harmful in a strongly statically-typed language like C++, because the **annotations get out of date** (the warts are just like comments and rot just like them) and they interfere with good use of the language (use the same name and overload resolution instead).

- Note: Some styles use very general (not type-specific) prefixes to denote the general use of a variable.
  - This is not harmful and does not fall under this guideline because it does not encode type information.
```cpp
auto p = new User();
auto p = make_unique<User>();
// note: "p" is not being used to say "raw pointer to type User,"
//       just generally to say "this is an indirection"

auto cntHits = calc_total_of_hits(/*...*/);
// note: "cnt" is not being used to encode a type,
//       just generally to say "this is a count of something"
```

- Note: Some styles distinguish members from local variable, and/or from global variable.
  - This is not harmful and does not fall under this guideline because it does not encode type information.
```cpp
struct S {
    int m_;
    S(int m) : m_{abs(m)} { }
};
```

- Note: Like C++, some styles distinguish types from non-types.
  - For example, by capitalizing type names, but not the names of functions and variables.
  - This is not harmful and does not fall under this guideline because it does not encode type information.

```cpp
typename<typename T>
class HashTable {   // maps string to T
    // ...
};

HashTable<int> index;
```

## NL.7: Make the length of a name roughly proportional to the length of its scope
- Rationale: The larger the scope the greater the chance of confusion and of an unintended name clash.

```cpp
double sqrt(double x);   // return the square root of x; x must be non-negative
int length(const char* p);  // return the number of characters in a zero-terminated C-style string
int length_of_string(const char zero_terminated_array_of_char[])    // bad: verbose
int g;      // bad: global variable with a cryptic name
int open;   // bad: global variable with a short, popular name
```
- The use of `p` for pointer and `x` for a floating-point variable is conventional and non-confusing in a restricted scope.


## NL.8: Use a consistent naming style
- Rationale: Consistence in naming and naming style increases readability.
- Note: There are many styles and when you use multiple libraries, you can't follow all their different conventions.
  - Choose a "house style", but leave "imported" libraries with their original style.

- Note: Try to be consistent in your use of acronyms and lengths of identifiers:
```cpp
int mtbf {12};
int mean_time_between_failures {12}; // make up your mind
```

## NL.9: Use ALL_CAPS for macro names only
- To avoid confusing macros with names that obey scope and type rules.

```cpp
void f() {
    const int SIZE{1000}; // Bad, use 'size' instead
    int v[SIZE];
}
```
- Note: This rule applies to non-macro symbolic constants:
```cpp
enum bad { BAD, WORSE, HORRIBLE }; // BAD
```

## NL.10: Prefer underscore_style names
- Reason: The use of underscores to separate parts of a name is the original C and C++ style and used in the C++ Standard Library.
- Note: This rule is a default to use only if you have a choice.
- Often, you don't have a choice and must follow an established style for consistency. **The need for consistency beats personal taste.**
- This is a recommendation for when you have no constraints or better ideas. This rule was added after many requests for guidance.

## NL.11: Make literals readable
- Example: Use digit separators to avoid long strings of digits
```cpp
auto c = 299'792'458; // m/s2
auto q2 = 0b0000'1111'0000'0000;
auto ss_number = 123'456'7890;
```
- Example: Use literal suffixes where clarification is needed
```cpp
auto hello = "Hello!"s; // a std::string
auto world = "world";   // a C-style string
auto interval = 100ms;  // using <chrono>
```
- Note: Literals should not be sprinkled all over the code as "magic constants", but it is still a good idea to make them readable where they are defined. It is easy to make a typo in a long string of integers.

## NL.15: Use spaces sparingly
- Too much space makes the text larger and distracts.
- Note: Some IDEs have their own opinions and add distracting space.
- This is a recommendation for when you have no constraints or better ideas.
- Note: We value well-placed whitespace as a significant help for readability. Just don't overdo it.

## NL.16: Use a conventional class member declaration order
- A conventional order of members improves readability.
- When declaring a class use the following order
  - **types: classes, enums, and aliases (using)**
  - **constructors, assignments, destructor**
  - **functions**
  - **data**
- Use the `public` before `protected` before `private` order.
- This is a recommendation for when you have no constraints or better ideas. This rule was added after many requests for guidance.

```cpp
class X {
public:
    // interface
protected:
    // unchecked function for use by derived class implementations
private:
    // implementation details
};
```
- Example: Sometimes, the default order of members conflicts with a desire to separate the public interface from implementation details.
  - In such cases, `private` types and functions can be placed with `private` data.
```cpp
class X {
public:
    // interface
protected:
    // unchecked function for use by derived class implementations
private:
    // implementation details (types, functions, and data)
};
```
- Example, bad: **Avoid multiple blocks of declarations of one access** (e.g., `public`) dispersed among blocks of declarations with different access (e.g. `private`).
```cpp
class X {   // bad
public:
    void f();
public:
    int g();
    // ...
};
```
- The use of macros to declare groups of members often leads to violation of any ordering rules. However, using macros obscures what is being expressed anyway.

## NL.17: Use K&R-derived layout
- This is the original C and C++ layout. It preserves vertical space well. It distinguishes different language constructs (such as functions and classes) well.
- This is a recommendation for when you have no constraints or better ideas. This rule was added after many requests for guidance.

```cpp
// The { for a class and a struct is not on a separate line, but the { for a function is.
struct Cable //Capitalize the names of your user-defined types to distinguish them from standards-library types.
{
    int x;
    // ...
};

double foo(int x) { //Do not capitalize function names.
    if (0 < x) { // the space between if and (
        // ...
    }

    switch (x) {
    case 0:
        // ...
        break;
    case amazing:
        // ...
        break;
    default:
        // ...
        break;
    }

    //Use separate lines for each statement, the branches of an if, and the body of a for.
    if (0 < x)
        ++x;

    if (x < 0)
        something();
    else
        something_else();

    return some_value;
}
```

## NL.18: Use C++-style declarator layout
- The C-style layout emphasizes use in expressions and grammar, whereas the C++-style emphasizes types.
- The use in expressions argument doesn't hold for references.

```cpp
T& operator[](size_t);   // OK
T &operator[](size_t);   // just strange
T & operator[](size_t);   // undecided
```
- Note: This is a recommendation for when you have no constraints or better ideas. This rule was added after many requests for guidance.


## NL.19: Avoid names that are easily misread
- Not everyone has screens and printers that make it easy to distinguish all characters.
  - We easily confuse similarly spelled and slightly misspelled words.
```cpp
int oO01lL = 6; // bad

int splunk = 7;
int splonk = 8; // bad: splunk and splonk are easily confused
```


## NL.20: Don't place two statements on the same line
   It is really easy to overlook a statement when there is more on a line.
```cpp
int x = 7; char* p = 29;    // don't
int x = 7; f(x);  ++x;      // don't
```

## NL.21: Declare one name (only) per declaration
- Minimizing confusion with the declarator syntax.


## NL.25: Don't use `void` as an argument type
- It's verbose and only needed where C compatibility matters.

```cpp
void f(void);   // bad
void g();       // better
```

## NL.26: Use conventional `const` notation
- Conventional notation is more familiar to more programmers. Consistency in large code bases.

```cpp
const int x = 7;    // OK
int const y = 9;    // bad

const int *const p = nullptr;   // OK, constant pointer to constant int
int const *const p = nullptr;   // bad, constant pointer to constant int
```

- We are well aware that you could claim the "bad" examples more logical than the ones marked "OK", but they also confuse more people, especially novices relying on teaching material using the far more common, conventional OK style.
- As ever, remember that the aim of these naming and layout rules is consistency and that aesthetics vary immensely.