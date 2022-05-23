# Enum: Enumerations

- [Enum: Enumerations](#enum-enumerations)
  - [Enum.1: Prefer enumerations over macros](#enum1-prefer-enumerations-over-macros)
  - [Enum.2: Use enumerations to represent sets of related named constants](#enum2-use-enumerations-to-represent-sets-of-related-named-constants)
  - [Enum.3: Prefer class enums over "plain" enums](#enum3-prefer-class-enums-over-plain-enums)
  - [Enum.4: Define operations on enumerations for safe and simple use](#enum4-define-operations-on-enumerations-for-safe-and-simple-use)
  - [Enum.5: Don't use ALL_CAPS for enumerators](#enum5-dont-use-all_caps-for-enumerators)
  - [Enum.6: Avoid unnamed enumerations](#enum6-avoid-unnamed-enumerations)
  - [Enum.7: Specify the underlying type of an enumeration only when necessary](#enum7-specify-the-underlying-type-of-an-enumeration-only-when-necessary)
  - [Enum.8: Specify enumerator values only when necessary](#enum8-specify-enumerator-values-only-when-necessary)

## Enum.1: Prefer enumerations over macros
- Macros do not obey scope and type rules. Also, macro names are removed during preprocessing and so usually don't appear in tools like debuggers.
- Also use an enum class to avoid name clashes.

```cpp
//------------  Bad  ------------
// some_header1.h
#define RED   0xFF0000
#define GREEN 0x00FF00
#define BLUE  0x0000FF

// some_header2.h
#define RED    0
#define PURPLE 1
#define BLUE   2

int webby = BLUE;   // webby == 2; probably not what was desired

//------------  Prefer  ------------
enum class Web_color { red = 0xFF0000, green = 0x00FF00, blue = 0x0000FF };
enum class Product_info { red = 0, purple = 1, blue = 2 };

int webby = blue;   // error: be specific
Web_color webby = Web_color::blue;
```

## Enum.2: Use enumerations to represent sets of related named constants
- An enumeration shows the enumerators to be related and can be a named type.
- Switching on an enumeration is common and the compiler can warn against unusual patterns of case labels. For example:
```cpp
enum class Product_info { red = 0, purple = 1, blue = 2 };

void print(Product_info inf) {
    switch (inf) {
    case Product_info::red: cout << "red"; break;
    case Product_info::purple: cout << "purple"; break;
    // for got to add case Product_info::blue!!! compiler might warn you though!
    }
}
```

## Enum.3: Prefer class enums over "plain" enums
- To minimize surprises: traditional enums convert to int too readily.

## Enum.4: Define operations on enumerations for safe and simple use
- Convenience of use and avoidance of errors.
```cpp
enum Day { mon, tue, wed, thu, fri, sat, sun };

Day& operator++(Day& d)
{
    return d = (d == Day::sun) ? Day::mon : static_cast<Day>(static_cast<int>(d)+1);
}

Day today = Day::sat;
Day tomorrow = ++today;
```

## Enum.5: Don't use ALL_CAPS for enumerators
- Avoid clashes with macros.
```cpp
 // webcolors.h (third party header)
#define RED   0xFF0000
#define GREEN 0x00FF00
#define BLUE  0x0000FF

// productinfo.h
// The following define product subtypes based on color

enum class Product_info { RED, PURPLE, BLUE };   // syntax error
```

## Enum.6: Avoid unnamed enumerations
- If you can't name an enumeration, the values are not related.
- Alternative: Use `constexpr` values instead.
```cpp
enum { red = 0xFF0000, scale = 4, is_signed = 1 }; // bad, unrelated unnamed enums

// alright!
constexpr int red = 0xFF0000;
constexpr short scale = 4;
constexpr bool is_signed = true;
```

## Enum.7: Specify the underlying type of an enumeration only when necessary
- The default is the easiest to read and write. int is the default integer type. int is compatible with C enums.
```cpp
enum class Direction : char { n, s, e, w,
                              ne, nw, se, sw };  // underlying type saves space

enum class Web_color : int32_t { red   = 0xFF0000,
                                 green = 0x00FF00,
                                 blue  = 0x0000FF };  // underlying type is redundant
```
- Note: Specifying the underlying type is necessary in forward declarations of enumerations:
```cpp
enum Flags : char;

void f(Flags);

// ....

enum Flags : char { /* ... */ };
```

## Enum.8: Specify enumerator values only when necessary
- It's the simplest. It avoids duplicate enumerator values. The default gives a consecutive set of values that is good for switch-statement implementations.
```cpp
enum class Col1 { red, yellow, blue };
enum class Col2 { red = 1, yellow = 2, blue = 2 }; // typo
enum class Month { jan = 1, feb, mar, apr, may, jun,
                   jul, august, sep, oct, nov, dec }; // starting with 1 is conventional
enum class Base_flag { dec = 1, oct = dec << 1, hex = dec << 2 }; // set of bits
```
- Specifying values is necessary to match conventional values (e.g., Month) and where consecutive values are undesirable (e.g., to get separate bits as in Base_flag).