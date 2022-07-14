# I: Interfaces
- An interface is a **contract between two parts of a program**.
  - Precisely stating what is expected of a supplier of a service and a user of that service is essential.
  - Having good (easy-to-understand, encouraging efficient use, not error-prone, supporting testing, etc.) **interfaces is probably the most important single aspect of code organization.**

## I.1: Make interfaces explicit
- Correctness. Assumptions not stated in an interface are easily overlooked and hard to test.
- Controlling the behavior of a function through a global (namespace scope) variable (a call mode) is implicit and potentially confusing.
- For example:
```cpp
//Example, bad
int round(double d)
{
    return (round_up) ? ceil(d) : d;    // don't: "invisible" dependency
}
```
- It will not be obvious to a caller that the meaning of two calls of round(7.2) might give different results.
- Exception: Sometimes we control the details of a set of operations by an environment variable, e.g., normal vs. verbose output or debug vs. optimized. The use of a non-local control is potentially confusing, but controls only implementation details of otherwise fixed semantics.

- Example, bad: Reporting through non-local variables (e.g., `errno`) is easily ignored. For example:
```cpp
// don't: no test of printf's return value
fprintf(connection, "logging: %d %d %d\n", x, y, s);
```
- What if the connection goes down so that no logging output is produced?
- Alternative: Throw an exception. An exception cannot be ignored.
- Alternative formulation: Avoid passing information across an interface through non-local or implicit state.
  - Note that non-const member functions pass information to other member functions through their object's state.
- Alternative formulation: An interface should be a function or a set of functions.
  - Functions can be function templates
  - sets of functions can be classes or class templates.

## I.2: Avoid non-const global variables
- Non-const global variables **hide dependencies and make the dependencies subject to unpredictable changes.**.

- The rule is "avoid", not "don't use." Of course there will be (rare) exceptions, such as cin, cout, and cerr.
- Warning: **The initialization of global objects is not totally ordered.** If you use a global object initialize it with a constant.
- Note that **it is possible to get undefined initialization order even for const objects.**
- he rule against global variables applies to namespace scope variables as well.
- Alternative: If you use global (more generally namespace scope) data to avoid copying, consider passing the data as an object by reference to const.
- Another solution is to define the data as the state of some object and the operations as member functions.
- Warning: Beware of data races: If one thread can access non-local data (or data passed by reference) while another thread executes the callee, we can have a data race. Every pointer or reference to mutable data is a potential data race.
  - You cannot have a race condition on immutable data.

## I.3: Avoid singletons
- Singletons are basically complicated global objects in disguise.
- If you don't want a global object to change, declare it `const` or `constexpr`.
- Exception: You can use the simplest "singleton" to get initialization on first use, if any, e.g.
```cpp
X& myX()
{
    static X my_x {3};
    return my_x;
}
```
- This is one of the most effective solutions to problems related to initialization order. In a multi-threaded environment, the initialization of the static object does not introduce a race condition (unless you carelessly access a shared object from within its constructor).
- Note that the initialization of a local static does not imply a race condition. However, if the destruction of X involves an operation that needs to be synchronized we must use a less simple solution.
```cpp
X& myX()
{
    static auto p = new X {3};
    return *p;  // potential leak
}
```
- Now someone must delete that object in some suitably thread-safe way. That's error-prone, so we don't use that technique unless
  - `myX` is in multi-threaded code,
  - that `X` object needs to be destroyed (e.g., because it releases a resource), and
  - `X`'s destructor's code needs to be synchronized.
- If you, as many do, define a singleton as a class for which only one object is created, functions like `myX` are not singletons, and this useful technique is not an exception to the no-singleton rule.

## I.4: Make interfaces precisely and strongly typed
- Types are the simplest and best documentation, improve legibility due to their well-defined meaning, and are checked at compile time.
- Also, precisely typed code is often optimized better.
```cpp
//Example, don't
void pass(void* data);    // weak and under qualified type void* is suspicious
```
- Callers are unsure what types are allowed and if the data may be mutated as `const` is not specified.
- Note all pointer types implicitly convert to `void*`, so it is easy for callers to provide this value.
- The callee must `static_cast` data to an unverified type to use it. That is error-prone and verbose.
- Only use `const void*` for passing in data in designs that are **indescribable**
  - Consider using a `variant` or a pointer to base instead.
- Alternative: Often, a template parameter can eliminate the `void* `turning it into a `T*` or `T&`.
  - For generic code these `Ts` can be general or concept constrained template parameters.


```cpp
//Example, bad

draw_rect(100, 200, 100, 500); // what do the numbers specify?
draw_rect(p.x, p.y, 10, 20); // what units are 10 and 20 in?
```
- It is clear that the caller is describing a rectangle, but it is unclear what parts they relate to.
- Also, an int can carry arbitrary forms of information, including values of many units, so we must guess about the meaning of the four ints.
- Comments and parameter names can help, but we could be explicit:
```cpp
void draw_rectangle(Point top_left, Point bottom_right);
void draw_rectangle(Point top_left, Size height_width);

draw_rectangle(p, Point{10, 20});  // two corners
draw_rectangle(p, Size{10, 20});   // one corner and a (height, width) pair
```
- Obviously, we cannot catch all errors through the static type system (e.g., the fact that a first argument is supposed to be a top-left point is left to convention (naming and comments)).

```cpp
//Example, bad
set_settings(true, false, 42); // what do the numbers specify?
```
- The parameter types and their values do not communicate what settings are being specified or what those values mean.

```cpp
//This design is more explicit, safe and legible:
alarm_settings s{};
s.enabled = true;
s.displayMode = alarm_settings::mode::spinning_light;
s.frequency = alarm_settings::every_10_seconds;
set_settings(s);
```
- For the case of a set of boolean values consider using a flags enum; a pattern that expresses a set of boolean values.
```cpp
enable_lamp_options(lamp_option::on | lamp_option::animate_state_transitions);
```
- Example, bad: In the following example, it is not clear from the interface what `time_to_blink` means: Seconds? Milliseconds?
```cpp
void blink_led(int time_to_blink) // bad -- the unit is ambiguous
{
    // ...
    // do something with time_to_blink
    // ...
}

void use() { blink_led(2); }
```
```cpp
// Example, good
// std::chrono::duration types helps making the unit of time duration explicit.

void blink_led(milliseconds time_to_blink) // good -- the unit is explicit
{
    // ...
    // do something with time_to_blink
    // ...
}

void use() { blink_led(1500ms); }
```
- The function can also be written in such a way that it will accept any time duration unit.

```cpp
template <class rep, class period>
void blink_led(duration<rep, period> time_to_blink) // good -- accepts any unit
{
    // assuming that millisecond is the smallest relevant unit
    auto milliseconds_to_blink = duration_cast<milliseconds>(time_to_blink);
    // ...
    // do something with milliseconds_to_blink
    // ...
}

void use() {
    blink_led(2s);
    blink_led(1500ms);
}
```

## I.5: State preconditions (if any)
- Arguments have meaning that might constrain their proper use in the callee.
```cpp
double sqrt(double x);
```
- Here x must be non-negative. The type system cannot (easily and naturally) express that, so we must use other means.
- For example:
```cpp
double sqrt(double x); // x must be non-negative
```
- Some preconditions can be expressed as assertions. For example:
```cpp
double sqrt(double x) { gsl::Expects(x >= 0); /* ... */ }
```
- Ideally, that `gsl::Expects(x >= 0)` should be part of the interface of `sqrt()` but that's not easily done.
- For now, we place it in the definition (function body).
- Note: Prefer a formal specification of requirements, such as `gsl::Expects(p);`.
  - If that is infeasible, use English text in comments, such as // the sequence [p:q) is ordered using <.

## I.6: Prefer `Expects()` for expressing preconditions
- To make it clear that the condition is a precondition and to enable tool use.

```cpp
int area(int height, int width)
{
    Expects(height > 0 && width > 0);            // good
    if (height <= 0 || width <= 0) my_error();   // obscure
    // ...
}
```
- Note: Preconditions can be stated in many ways, including comments, if-statements, and `assert()`.
- This can make them hard to distinguish from ordinary code, hard to update, hard to manipulate by tools, and might have the wrong semantics (do you always want to abort in debug mode and check nothing in productions runs?).
- Note: Preconditions should be part of the interface rather than part of the implementation, but we don't yet have the language facilities to do that.
  - Once language support becomes available (e.g., see the `contract` proposal) we will adopt the standard version of preconditions, postconditions, and assertions.
- Note: `Expects()` can also be used to check a condition in the middle of an algorithm.
- Note: No, using `unsigned` is not a good way to sidestep the problem of ensuring that a value is non-negative.


## I.7: State postconditions
- To detect misunderstandings about the result and possibly catch erroneous implementations.

```cpp
//Example, bad
int area(int height, int width) { return height * width; }  // bad
```
- Here, we (incautiously) left out the precondition specification, so it is not explicit that height and width must be positive.
- We also left out the postcondition specification, so it is not obvious that the algorithm (height * width) is wrong for areas larger than the largest integer. Overflow can happen. Consider using:

```cpp
int area(int height, int width)
{
    auto res = height * width;
    Ensures(res > 0);
    return res;
}
// Example, bad
// Consider a famous security bug:

void f()    // problematic
{
    char buffer[MAX];
    // ...
    memset(buffer, 0, sizeof(buffer));
}
```
- There was no postcondition stating that the buffer should be cleared and the optimizer eliminated the apparently redundant `memset()` call:
```cpp
void f() // better
{
    char buffer[MAX];
    // ...
    memset(buffer, 0, sizeof(buffer));
    Ensures(buffer[0] == 0);
}
```
- Note: Postconditions are often informally stated in a comment that states the purpose of a function; `Ensures()` can be used to make this more systematic, visible, and checkable.
- Note: Postconditions are especially important when they relate to something that is not directly reflected in a returned result, such as a state of a data structure used.
- Example: Consider a function that manipulates a `Record`, using a `mutex` to avoid race conditions:
```cpp
mutex m;
void manipulate(Record& r)    // don't
{
    m.lock();
    // ... no m.unlock() ...
}
```
- Here, we "forgot" to state that the mutex should be released, so we don't know if the failure to ensure release of the mutex was a bug or a feature. 
- Stating the postcondition would have made it clear:
```cpp
void manipulate(Record& r)    // postcondition: m is unlocked upon exit
{
    m.lock();
    // ... no m.unlock() ...
}
```
- The bug is now obvious (but only to a human reading comments).
- Better still, use RAII to ensure that the postcondition ("the lock must be released") is enforced in code:
```cpp
void manipulate(Record& r)    // best
{
    lock_guard<mutex> _ {m};
    // ...
}
```
- Note: Ideally, postconditions are stated in the interface/declaration so that users can easily see them.
  - Only postconditions related to the users can be stated in the interface.
  - Postconditions related only to internal state belongs in the definition/implementation.


## I.8: Prefer `Ensures()` for expressing postconditions
- To make it clear that the condition is a postcondition and to enable tool use.

```cpp
void f()
{
    char buffer[MAX];
    // ...
    memset(buffer, 0, MAX);
    Ensures(buffer[0] == 0);
}
```
- Note: Postconditions can be stated in many ways, including comments, if-statements, and assert(). This can make them hard to distinguish from ordinary code, hard to update, hard to manipulate by tools, and might have the wrong semantics.
- Alternative: Postconditions of the form "this resource must be released" are best expressed by RAII.
- Note: Ideally, that `Ensures` should be part of the interface, but that's not easily done. For now, we place it in the definition (function body). Once language support becomes available (e.g., see the contract proposal) we will adopt the standard version of preconditions, postconditions, and assertions.

## I.9: If an interface is a template, document its parameters using concepts
- Make the interface precisely specified and compile-time checkable in the (not so distant) future.
- Example: Use the C++20 style of requirements specification. For example:
```cpp
template <typename Iter, typename Val>
requires input_iterator<Iter> && equality_comparable_with<iter_value_t<Iter>, Val>
Iter find(Iter first, Iter last, Val v) {
    // ...
}
```

## I.10: Use exceptions to signal a failure to perform a required task
- It should not be possible to ignore an error because that could leave the system or a computation in an undefined (or unexpected) state. This is a major source of errors.

```cpp
int printf(const char*...); // bad: return negative number if output fails

template <class F, class... Args>
// good: throw system_error if unable to start the new thread
explicit thread(F&& f, Args&&... args);
```
- Note: What is an error?
  - An error means that **the function cannot achieve its advertised purpose (including establishing postconditions)**.
  - Calling code that ignores an error could lead to wrong results or undefined systems state.
  - For example, not being able to connect to a remote server is not by itself an error:
    - the server can refuse a connection for all kinds of reasons, so the natural thing is to return a result that the caller should always check.
    - However, if failing to make a connection is considered an error, then a failure should throw an exception.
- Exception: Many traditional interface functions (e.g., UNIX signal handlers) use error codes (e.g., `errno`) to report what are really status codes, rather than errors. You don't have a good alternative to using such, so calling these does not violate the rule.

- Alternative: If you can't use exceptions (e.g., because your code is full of old-style raw-pointer use or because there are hard-real-time constraints), consider using a style that returns a pair of values:
```cpp
int val;
int error_code;
tie(val, error_code) = do_something();
if (error_code) {
    // ... handle the error or exit ...
}
// ... use val ...
```
- This style unfortunately leads to uninitialized variables. Since C++17 the "structured bindings" feature can be used to initialize variables directly from the return value:
```cpp
auto [val, error_code] = do_something();
if (error_code) {
    // ... handle the error or exit ...
}
// ... use val ...
```
- Note: We don't consider "performance" a valid reason not to use exceptions.
- Often, explicit error checking and handling consume as much time and space as exception handling.
- Often, cleaner code yields better performance with exceptions (simplifying the tracing of paths through the program and their optimization).
- A good rule for performance critical code is to move checking outside the critical part of the code.
  - In the longer term, more regular code gets better optimized.
  - Always carefully measure before making performance claims.



## I.25: Prefer empty abstract classes as interfaces to class hierarchies
- not yet read
## I.27: For stable library ABI, consider the Pimpl idiom
- not yet read