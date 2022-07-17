# SL: The Standard Library
- Using only the bare language, every task is tedious (in any language). Using a suitable library any task can be reasonably simple.
- The standard library has steadily grown over the years. Its description in the standard is now larger than that of the language features.

## SL.1: Use libraries wherever possible
- Save time. Don't re-invent the wheel. Don't replicate the work of others. Benefit from other people's work when they make improvements. Help other people when you make improvements.


## SL.2: Prefer the standard library to other libraries
- More people know the standard library. It is more likely to be stable, well-maintained, and widely available than your own code or most other libraries.

## SL.3: Do not add non-standard entities to namespace std
- Adding to `std` might change the meaning of otherwise standards conforming code. Additions to `std` might clash with future versions of the standard.

## SL.4: Use the standard library in a type-safe manner
- Because, obviously, breaking this rule can lead to undefined behavior, memory corruption, and all kinds of other bad errors.
- Note: This is a semi-philosophical meta-rule, which needs many supporting concrete rules. We need it as an umbrella for the more specific rules.

# SL.regex: Regex
- `<regex>` is the standard C++ regular expression library. It supports a variety of regular expression pattern conventions.

# SL.chrono: Time
- `<chrono>` (defined in namespace `std::chrono`) provides the notions of `time_point` and `duration` together with functions for outputting time in various units.
- It provides clocks for registering `time_points`.

# Subsections
- [SL.con: Containers](SL.con.md)
- [SL.str: String](SL.str.md)
- [SL.io: Iostream](SL.io.md)