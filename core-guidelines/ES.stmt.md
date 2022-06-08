# ES.stmt: Statements

## ES.70: Prefer a switch-statement to an if-statement when there is a choice
- Readability.
- **Efficiency: A switch compares against constants and is usually better optimized than a series of tests in an if-then-else chain.**
- A switch enables some heuristic consistency checking. For example, have all values of an enum been covered? If not, is there a default?
```cpp
void use(int n) {
    switch (n) { // good
    case 0:
        // ...
        break;
    case 7:
        // ...
        break;
    default:
        // ...
        break;
    }
}
```
- rather than:
```cpp
void use2(int n) {
    if (n == 0) // bad: if-then-else chain comparing against a set of constants
                // ...
    else if (n == 7)
    // ...
}
```

## ES.71: Prefer a range-for-statement to a for-statement when there is a choice
- Readability. Error prevention. Efficiency.

```cpp
for (gsl::index i = 0; i < v.size(); ++i) // bad
    cout << v[i] << '\n';

for (auto p = v.begin(); p != v.end(); ++p) // bad
    cout << *p << '\n';

for (auto& x : v) // OK
    cout << x << '\n';

for (gsl::index i = 1; i < v.size(); ++i) // touches two elements: can't be a range-for
    cout << v[i] + v[i - 1] << '\n';

for (gsl::index i = 0; i < v.size(); ++i) // possible side effect: can't be a range-for
    cout << f(v, &v[i]) << '\n';

for (gsl::index i = 0; i < v.size(); ++i) { // body messes with loop variable: can't be a range-for
    if (i % 2 != 0)
        cout << v[i] << '\n'; // output odd elements
}
```
- A human or a good static analyzer might determine that there really isn't a side effect on v in `f(v, &v[i])` so that the loop can be rewritten.
- "Messing with the loop variable" in the body of a loop is typically best avoided.

Note - Don't use expensive copies of the loop variable of a range-for loop:
```cpp
for (string s : vs) // ...
//This will copy each elements of vs into s. Better:

for (string& s : vs) // ...
//Better still, if the loop variable isn't modified or copied:

for (const string& s : vs) // ...
```

## ES.72: Prefer a for-statement to a while-statement when there is an obvious loop variable
- Readability: the complete logic of the loop is visible "up front". The scope of the loop variable can be limited.

```cpp
for (gsl::index i = 0; i < vec.size(); i++) {
    // do work
}
```
```cpp
int i = 0; //bad
while (i < vec.size()) {
    // do work
    i++;
}
```

## ES.73: Prefer a while-statement to a for-statement when there is no obvious loop variable
- Reason - Readability.

```cpp
int events = 0;
for (; wait_for_event(); ++events) {  // bad, confusing
    // ...
}
```
- The "event loop" is misleading because the events counter has nothing to do with the loop condition (`wait_for_event()`).

```cpp
int events = 0;
while (wait_for_event()) {      // better
    ++events;
    // ...
}
```

## ES.74: Prefer to declare a loop variable in the initializer part of a for-statement
- See [ES.6](ES.dcl.md#es6-declare-names-in-for-statement-initializers-and-conditions-to-limit-scope)

## ES.75: Avoid do-statementsReason
- Readability, avoidance of errors.
- The termination condition is at the end (where it can be overlooked) and the condition is not checked the first time through.
```cpp
Example
int x;
do {
    cin >> x;
    // ...
} while (x < 0);
```
- Note: Yes, there are genuine examples where a do-statement is a clear statement of a solution, but also many bugs.


## ES.76: Avoid goto
- Readability, avoidance of errors.
- There are better control structures for humans; `goto` is for machine generated code.
- Exception - Breaking out of a nested loop. In that case, always jump forwards.
```cpp
for (int i = 0; i < imax; ++i) {
    for (int j = 0; j < jmax; ++j) {
        if (a[i][j] > elem_max) goto finished;
        // ...
    }
}
finished:
```
```cpp
// Example, bad: There is a fair amount of use of the C goto-exit idiom:
void f()
{
    // ...
        goto exit;
    // ...
        goto exit;
    // ...
exit:
    // ... common cleanup code ...
}
```
- This is an ad-hoc simulation of destructors. Declare your resources with handles with destructors that clean up.
- If for some reason you cannot handle all cleanup with destructors for the variables used, consider `gsl::finally()` as a cleaner and more reliable alternative to goto exit

## ES.77: Minimize the use of break and continue in loops
- In a non-trivial loop body, it is easy to overlook a break or a continue.
- A break in a loop has a dramatically different meaning than a break in a switch-statement (and you can have switch-statement in a loop and a loop in a switch-case).

```cpp
switch(x) {
case 1 :
    while (/* some condition */) {
        // ...
    break;
    } // Oops! break switch or break while intended?
case 2 :
    // ...
    break;
}
```
- Alternative: Often, a loop that requires a break is a good candidate for a function (algorithm), in which case the break becomes a return.

```cpp
// Original code: break inside loop
void use1() {
    std::vector<T> vec = {/* initialized with some values */};
    T value;
    for (const T item : vec) {
        if (/* some condition*/) {
            value = item;
            break;
        }
    }
    /* then do something with value */
}

// BETTER: create a function and return inside loop
T search(const std::vector<T>& vec) {
    for (const T& item : vec) {
        if (/* some condition*/)
            return item;
    }
    return T(); // default value
}

```

```cpp
void use2()
{
    std::vector<T> vec = {/* initialized with some values */};
    T value = search(vec);
    /* then do something with value */
}
```
- Often, a loop that uses continue can equivalently and as clearly be expressed by an if-statement.

```cpp
for (int item : vec) {  // BAD
    if (item%2 == 0) continue;
    if (item == 5) continue;
    if (item > 10) continue;
    /* do something with item */
}

for (int item : vec) {  // GOOD
    if (item%2 != 0 && item != 5 && item <= 10) {
        /* do something with item */
    }
}
```
- Note: If you really need to break out a loop, a break is typically better than alternatives such as modifying the loop variable or a goto:


## ES.78: Don't rely on implicit fallthrough in switch statements
- Always end a non-empty case with a break. Accidentally leaving out a break is a fairly common bug.
- A deliberate fallthrough can be a maintenance hazard and should be rare and explicit.

```cpp
//Example
switch (eventType) {
case Information:
    update_status_bar();
    break;
case Warning:
    write_event_log();
    // Bad - implicit fallthrough
case Error:
    display_error_window();
    break;
}
```
- Multiple case labels of a single statement is OK:
```cpp
switch (x) {
case 'a':
case 'b':
case 'f':
    do_something(x);
    break;
}
```
- Return statements in a case label are also OK:
```cpp
switch (x) {
case 'a':
    return 1;
case 'b':
    return 2;
case 'c':
    return 3;
}
```
- Exceptions: In rare cases if fallthrough is deemed appropriate, be explicit and use the [[fallthrough]] annotation:
```cpp
switch (eventType) {
case Information:
    update_status_bar();
    break;
case Warning:
    write_event_log();
    [[fallthrough]];
case Error:
    display_error_window();
    break;
}
```

## ES.79: Use default to handle common cases (only)
- Code clarity. Improved opportunities for error detection.

```cpp
enum E { a, b, c, d };

void f1(E x) {
    switch (x) {
    case a:
        do_something();
        break;
    case b:
        do_something_else();
        break;
    default:
        take_the_default_action();
        break;
    }
}
```
- Here it is clear that there is a default action and that cases a and b are special.
- But what if there is no default action and you mean to handle only specific cases? In that case, have an empty default or else it is impossible to know if you meant to handle all cases:
```cpp
void f2(E x) {
    switch (x) {
    case a:
        do_something();
        break;
    case b:
        do_something_else();
        break;
    default:
        // do nothing for the rest of the cases
        break;
    }
}
```
- If you leave out the default, a maintainer and/or a compiler might reasonably assume that you intended to handle all cases:
```cpp
void f2(E x) {
    switch (x) {
    case a:
        do_something();
        break;
    case b:
    case c:
        do_something_else();
        break;
    }
}
```
- Did you forget case d or deliberately leave it out? Forgetting a case typically happens when a case is added to an enumeration and the person doing so fails to add it to every switch over the enumerators.
