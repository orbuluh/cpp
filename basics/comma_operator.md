
- [Quick fact](#quick-fact)
- [Comma operator force the order of execution:](#comma-operator-force-the-order-of-execution)
- [Not every comma is actually a comma operator](#not-every-comma-is-actually-a-comma-operator)
- [Magic: comma operator + variadic expansion + `std::initializer_list` can avoid recursive template instantiation](#magic-comma-operator--variadic-expansion--stdinitializer_list-can-avoid-recursive-template-instantiation)
- [Reference](#reference)

# Quick fact
* The comma operator:
  * **has the lowest precedence**
  * is **left-associative**
  * A default version of comma operator is **defined for all types (built-in and custom).**

# Comma operator force the order of execution:
  * The results of the first statement are fully evaluated before the next statement is executed.
  * it works as follows:
    * given `exprA , exprB`:
    * `exprA` is evaluated
    * the result of `exprA` is ignored
    * `exprB` is evaluated
    * the result of `exprB` is returned as the result of the whole.
  * With most operators, the compiler is allowed to choose the order of execution and it is even required to skip the execution whatsoever if it does not affect the final result (e.g. `false && foo()` will skip the call to `foo`).
  * This is however not the case for comma operator and the above steps will always happen.
    ```cpp
        // example 1:
        // The value of a will be b, but the value of the expression will be c.
        d = (a = b, c); // a would be equal to b and d would be equal to c

        // example 2:
        a = b, c;    // a is set to the value of b!
        a = (b, c);  // a is set to the value of c!
    ```
* In practice, the default comma operator works almost the same way as a semicolon. The difference is that two expressions separated by a semicolon form two separate statements, while **comma-separation keeps all as a single expression**.

# Not every comma is actually a comma operator
  * For all the cases below, we have no guarantee as to what order will be evaluated.
  * variable declaration list is comma separated, but these are not comma operators. E.g.:
    ```cpp
    int a, b;
    int a=5, b=3;
    ```
  * comma-separated argument list.
    ```cpp
    foo(x,y); //x and y can be evaluated in any order!
    ```
  * comma-separated macro argument list
    ```cpp
    FOO(x,y);
    ```
  * comma-separated template argument list
    ```cpp
    foo<a,b>;
    ```
  * comma-separated parameter list
    ```cpp
    int foo(int a, int b)
    ```
  * comma-separated initializer list in a class constructor
    ```cpp
    Foo::Foo() : a(5), b(3) {}
    ```

# Magic: comma operator + variadic expansion + `std::initializer_list` can avoid recursive template instantiation
- check `magicPrintCsv`, `magicMin` in [code](comma_op_magic.h)

# Reference
- [how-does-the-comma-operator-work](https://stackoverflow.com/questions/54142/how-does-the-comma-operator-work)
- [Folds (ish) In C++11](https://articles.emptycrate.com/2016/05/14/folds_in_cpp11_ish.html)