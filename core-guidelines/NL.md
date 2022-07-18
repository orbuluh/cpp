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
