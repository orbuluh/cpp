# F.call: Parameter passing

## F.15: Prefer simple and conventional ways of passing information
- Using "unusual and clever" techniques causes surprises, slows understanding by other programmers, and encourages bugs.
- If you really feel the need for an optimization beyond the common techniques, measure to ensure that it really is an improvement, and document/comment because the improvement might not be portable.
- The following tables summarize the advice in the following Guidelines, F.16-21.

- [Normal parameter passing](https://github.com/isocpp/CppCoreGuidelines/raw/master/param-passing-normal.png)
- [Advanced parameter passing](https://github.com/isocpp/CppCoreGuidelines/raw/master/param-passing-advanced.png)
  - Use the advanced techniques only after demonstrating need, and document that need in a comment.
- cheap (e.g. `int`) or impossible to copy (e.g. `unique_ptr`)
  - In: `f(X)`
  - In & retain "copy": `f(X)`
  - Out: `X f()`
  - In/Out: `f(X&)`
  - In & move from: `f(X&&)`

- cheap to move (e.g. `std::string`, `std::vector<T>`), or moderate cost to move (e.g. `std::array<std::vector>`, `BigPOD`) or don't know (unfamiliar type, template...)
  - In: `f(const X&)`
  - In & retain "copy": `f(const X&)` + `f(X&&)` & move
  - Out: `X f()`
  - In/Out: `f(X&)`
  - In & move from: `f(X&&)`

- Expensive to move (e.g. `BidPOD[`], `std::array<BigPOD, N>`)
  - In: `f(const X&)`
  - In & retain "copy": `f(const X&)`
  - Out: `f(X&)`
  - In/Out: `f(X&)`
  - In & move from: `f(X&&)`
- For passing sequences of characters see [String](SL.str.md).