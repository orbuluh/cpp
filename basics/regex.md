# [`std::regex`](https://en.cppreference.com/w/cpp/regex)

## Main concept


Almost all operations with `regex`es can be characterized by operating on several of the following objects:

**Target sequence**:

- The character sequence that is searched for a pattern.
- This may be a range specified by two iterators, a null-terminated character string or a std::string.

**Pattern**

- This is the regular expression itself. It determines what constitutes a match.
- It is an object of type `std::basic_regex`, constructed from a string with special syntax.
- See `regex_constants::syntax_option_type` for the description of supported syntax variations.

**Matched array**

- The information about matches may be retrieved as an object of type `std::match_results`.

**Replacement string**

- This is a string that determines how to replace the matches
- see `regex_constants::match_flag_type` for the description of supported syntax variations.



## Regex flavor

Six different regular expression flavors or grammars are defined in `std::regex_constants`:

- `ECMAScript`: Similar to [JavaScript](https://www.regular-expressions.info/javascript.html)
- `basic`: Similar to [POSIX BRE](https://www.regular-expressions.info/posix.html).
- `extended`: Similar to [POSIX ERE](https://www.regular-expressions.info/posix.html).
- `grep`: Same as basic, with the addition of treating line feeds as alternation operators.
- `egrep`: Same as extended, with the addition of treating line feeds as alternation operators.
- `awk`: Same as extended, with the addition of supporting common [escapes for non-printable characters](https://www.regular-expressions.info/nonprint.html).

> Mostly, use the ECMAScript grammar is the default grammar and offers far more features that the other grammars.

## Basics - search/match/catch group

`std::regex_search()`: check whether your regex can match any part of the string.

`std::regex_match()`: check whether your regex can match the *entire* subject string.&#x20;

- `std::regex` lacks anchors that exclusively match at the start and end of the string, you have to call `regex_match()` when using a regex to validate user input.

- `regex_search()` and `regex_match()` return just true or false.
- To get the parts of the string matched by capturing groups when using either function, you need to pass an object of the template class `std::match_results`

- `std::cmatch` when your subject is an array of `char`
- `std::smatch` when your subject is an `std::string` object
- `std::wcmatch` when your subject is an array of `wchar_t`
- `std::wsmatch` when your subject is an `std::wstring` object

When the function call returns true, you can call the `str()`, `position()`, and `lengt()` member functions of the `match_results` object to get the text that was matched, or the starting position and its length of the match relative to the subject string.

- Call these member functions without a parameter or with 0 as the parameter to get the overall regex match.
- Call them passing 1 or greater to get the match of a particular capturing group.;
- The size() member function indicates the number of capturing groups plus one for the overall match.

## Finding All

To find all regex matches in a string, you need to use an iterator.

- Construct an object of the template class `std::regex_iterator` using one of these four template instantiations:

  - `std::cregex_iterator` when your subject is an array of `char`
  - `std::sregex_iterator` when your subject is an `std::string` object
  - `std::wcregex_iterator` when your subject is an array of `wchar_t`
  - `std::wsregex_iterator` when your subject is an `std::wstring` object

## Replacing All

To replace all matches in a string, call `std::regex_replace()`

- The function returns a new string with the replacements applied.
