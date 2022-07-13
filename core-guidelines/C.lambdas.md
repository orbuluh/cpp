# C.lambdas: Function objects and lambdas
- A function object is an object supplying an `overloaded ()` so that you can call it.
- A lambda expression (colloquially often shortened to "a lambda") is a notation for **generating a function object**.
- **Function objects should be cheap to copy (and therefore passed by value).**

## [F.50: Use a lambda when a function won't do (to capture local variables, or to write a local function)](F.call.md#f50-use-a-lambda-when-a-function-wont-do-to-capture-local-variables-or-to-write-a-local-function)
## [F.52: Prefer capturing by reference in lambdas that will be used locally, including passed to algorithms](F.call.md#f52-prefer-capturing-by-reference-in-lambdas-that-will-be-used-locally-including-passed-to-algorithms)
## [F.53: Avoid capturing by reference in lambdas that will be used non-locally, including returned, stored on the heap, or passed to another thread](F.call.md#f53-avoid-capturing-by-reference-in-lambdas-that-will-be-used-non-locally-including-returned-stored-on-the-heap-or-passed-to-another-thread)
## [ES.28: Use lambdas for complex initialization, especially of const variables](ES.dcl.md#es28-use-lambdas-for-complex-initialization-especially-of-const-variables)