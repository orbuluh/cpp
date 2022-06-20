# CP.coro: Coroutines
- [CP.coro: Coroutines](#cpcoro-coroutines)
  - [CP.51: Do not use capturing lambdas that are coroutines](#cp51-do-not-use-capturing-lambdas-that-are-coroutines)
  - [CP.52: Do not hold locks or other synchronization primitives across suspension pointsReason](#cp52-do-not-hold-locks-or-other-synchronization-primitives-across-suspension-pointsreason)
  - [CP.53: Parameters to coroutines should not be passed by reference](#cp53-parameters-to-coroutines-should-not-be-passed-by-reference)

## CP.51: Do not use capturing lambdas that are coroutines
- Usage patterns that are correct with normal lambdas are hazardous with coroutine lambdas.
- The obvious pattern of capturing variables will result in accessing freed memory after the first suspension point, even for refcounted smart pointers and copyable types.

- A lambda results in a closure object with storage, **often on the stack, that will go out of scope at some point.**
- When the closure object goes out of scope the captures will also go out of scope.
- Normal lambdas will have finished executing by this time so it is not a problem.
- **Coroutine lambdas may resume from suspension after the closure object has destructed and at that point all captures will be use-after-free memory access.**
```cpp
// Example, Bad
int value = get_value();
std::shared_ptr<Foo> sharedFoo = get_foo();
{
    const auto lambda = [value, sharedFoo]() -> std::future<void> {
        co_await something();
        // "sharedFoo" and "value" have already been destroyed
        // the "shared" pointer didn't accomplish anything
    };
    lambda();
} // the lambda closure object has now gone out of scope

// Example, Better-----------------------------------------
int value = get_value();
std::shared_ptr<Foo> sharedFoo = get_foo();
{
    // take as by-value parameter instead of as a capture
    const auto lambda = [](auto sharedFoo, auto value) -> std::future<void> {
        co_await something();
        // sharedFoo and value are still valid at this point
    };
    lambda(sharedFoo, value);
} // the lambda closure object has now gone out of scope

//Example, Best-----------------------------------------
//Use a function for coroutines.
std::future<void> Class::do_something(int value, std::shared_ptr<Foo> sharedFoo)
{
    co_await something();
    // sharedFoo and value are still valid at this point
}

void SomeOtherFunction() {
    int value = get_value();
    std::shared_ptr<Foo> sharedFoo = get_foo();
    do_something(value, sharedFoo);
}
```


## CP.52: Do not hold locks or other synchronization primitives across suspension pointsReason
- This pattern creates a significant risk of deadlocks.
- Some types of waits will allow the current thread to perform additional work until the asynchronous operation has completed.
- If the thread holding the lock performs work that requires the same lock then it will deadlock because it is trying to acquire a lock that it is already holding.
- If the coroutine completes on a different thread from the thread that acquired the lock then that is undefined behavior.
- Even with an explicit return to the original thread an exception might be thrown before coroutine resumes and the result will be that the lock guard is not destructed.

```cpp
//Example, Bad

std::mutex g_lock;
std::future<void> Class::do_something() {
    std::lock_guard<std::mutex> guard(g_lock);
    co_await something(); // DANGER: coroutine has suspended execution while
                          // holding a lock
    co_await somethingElse();
}

//Example, Good

std::mutex g_lock;
std::future<void> Class::do_something() {
    {
        std::lock_guard<std::mutex> guard(g_lock);
        // modify data protected by lock
    }
    co_await something(); // OK: lock has been released before coroutine
                          // suspends
    co_await somethingElse();
}
```
- This pattern is also bad for performance. When a suspension point is reached, such as `co_await`, execution of the current function stops and other code begins to run.
- It may be a long period of time before the coroutine resumes. For that entire duration the lock will be held and cannot be acquired by other threads to perform work.

## CP.53: Parameters to coroutines should not be passed by reference

- Once a coroutine reaches the first suspension point, such as a `co_await`, the synchronous portion returns.
- After that point any parameters passed by reference are dangling.
- Any usage beyond that is undefined behavior which may include writing to freed memory.

```cpp
//Example, Bad
std::future<int> Class::do_something(const std::shared_ptr<int>& input) {
    co_await something();
    // DANGER: the reference to input may no longer be valid and may be freed
    // memory
    co_return *input + 1;
}

// Example, Good
std::future<int> Class::do_something(std::shared_ptr<int> input) {
    co_await something();
    co_return *input + 1; // input is a copy that is still valid here
}
```
- This problem does not apply to reference parameters that are only accessed before the first suspension point.
- Subsequent changes to the function may add or move suspension points which would reintroduce this class of bug.
- Some types of coroutines have the suspension point before the first line of code in the coroutine executes, in which case reference parameters are always unsafe.
- It is safer to always pass by value because the copied parameter will live in the coroutine frame that is safe to access throughout the coroutine.
- The same danger applies to output parameters. [F.20](F.md#f20-for-out-output-values-prefer-return-values-to-output-parameters) discourages output parameters.
- Coroutines should avoid them entirely.

