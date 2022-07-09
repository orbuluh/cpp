# F.call: Parameter passing

## F.15: Prefer simple and conventional ways of passing information
- Using "unusual and clever" techniques causes surprises, slows understanding by other programmers, and encourages bugs.
- If you really feel the need for an optimization beyond the common techniques, measure to ensure that it really is an improvement, and document/comment because the improvement might not be portable.
- The following tables summarize the advice in the following Guidelines, F.16-21.

- [Normal parameter passing](https://github.com/isocpp/CppCoreGuidelines/raw/master/param-passing-normal.png)
- [Advanced parameter passing](https://github.com/isocpp/CppCoreGuidelines/raw/master/param-passing-advanced.png)
- Use the advanced techniques only after demonstrating need, and document that need in a comment.