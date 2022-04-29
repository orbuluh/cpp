# Quick fact
- Once you’ve started your thread, you need to explicitly decide whether to wait for it to finish or leave it to run on its own
- If you don’t decide before the `std::thread` object is destroyed, then your program is terminated (the `std::thread` destructor calls `std::terminate()`) It’s therefore imperative that you ensure that the thread is correctly joined or detached, even in the presence of exceptions.
  - Note that you only have to make this decision **before** the std::thread object is destroyed
  - the thread itself may well have finished long before you join with it or detach it
  - if you **detach** it, then if the thread is still running, it will continue to do so, and may continue running long after the std::thread object is destroyed; **it will only stop running when it finally returns from the thread function.**
    - If you don’t wait for your thread to finish, you need to ensure that the data accessed by the thread is valid until the thread has finished with it
    - In particular, it’s a **bad idea to create a thread within a function that has access to the local variables in that function, unless the thread is guaranteed to finish before the function exits.**
  - Alternatively, you can ensure that the thread has completed execution before the function exits by joining with the thread.

# Waiting for a thread to complete
- The act of calling `join()` cleans up any storage associated with the thread, so the `std::thread` object is no longer associated with the now-finished thread; it isn’t associated with any thread.
- This means that you can call `join()` only once for a given thread; once you’ve called `join()`, the std::thread object is no longer joinable, and `joinable()` will return false.
- If you need more fine-grained control over waiting for a thread ...
  - to check whether a thread is finished --> condition variables
  - to wait only a certain period of time --> futures

# Waiting in exceptional circumstances
- the call to `join()` is liable to be skipped if an exception is thrown after the thread has been started but before the call to `join()`.
- To avoid your application being terminated when an exception is thrown, you therefore need to make a decision about what to do in this case.
- In general, if you were intending to call `join()` in a non-exceptional case, you also need to call `join()` in the presence of an exception to avoid accidental lifetime problems.
- It’s important to ensure this is the case for all possible exit paths, whether normal or exceptional, and it’s desirable to provide a simple, concise mechanism for doing so - Ideally, RAII - check [example](RaiiThread.h)

# `detach()` thread to the background
- If you don’t need to wait for a thread to finish, you can avoid this exception-safety issue by detaching it.
  - This breaks the association of the thread with the `std::thread` object and
  - ensures that `std::terminate()` won’t be called when the `std::thread` object is destroyed, even though the thread is still running in the background.
- if a thread becomes detached,
  - no direct means of communicating with the detached thread
  - It’s no longer possible to wait for that thread to complete
  - It's also impossible to obtain a `std::thread` object that references it, so it can no longer be joined.
- Detached threads truly run in the background;
  - ownership and control are passed over to the C++ Runtime Library, which ensures that the resources associated with the thread are correctly reclaimed when the thread exits.
- Two possible use cases:
  1. Detached tasks are typically long-running; they run for almost the entire lifetime of the application, performing a background task such as monitoring the filesystem, clearing unused entries out of object caches, or optimizing data structures.
  2. It may make sense to use a detached thread where there’s another mechanism for identifying when the thread has completed or where the thread is used for a **fire-and-forget** task.
- you can only call `t.detach()` for a `std::thread` object `t` when `t.joinable()` returns true.

# Passing arguments to a thread
- By default, the arguments are **copied** into internal storage, **and then** passed to the callable object or function **as rvalues** as if they were temporaries.
  - Note there is an "and then" - these are 2 steps.
```cpp
void f(int i, std::string const& s);
// buffer pointer is copied into internal
// storage first, and then before passing
// into f(...), it needs to be converted to
// std::string -> which could happen after
// oops exit - which cause buffer to be
// destroyed before conversion and crash!
void oops(int some_param)
{
  char buffer[1024];
  sprintf(buffer, "%i", some_param);
	std::thread t(f, 3, buffer);
  t.detach();
}
```
  - To fix above, you need to cast to do: `std::thread t(f, 3, std::string(buffer));`, so it cast and copy before passing into thread internal storage for the input parameters.

## Pass reference to thread so input would change? `std::ref()`
- Note the aforementioned 2 steps, your input is passed as rvalue to the callable - this means that your callable can't have a signature with non-const lvalue reference - as **rvalue can't bind to a non-const lvalue reference.**
- The solution is that you need to wrap the arguments that need to be references in `std::ref`

## Pass move-only data to thread?
```cpp
void process_big_object(std::unique_ptr<big_object>);
std::unique_ptr<big_object> p(new big_object);
//...
std::thread t(process_big_object, std::move(p));
```
* When such source object is temporary, the move is automatic, but where the source is a named value, the transfer must be requested directly by invoking `std::move()`

# Transferring ownership of a thread
* many resource-owning types in the C++ Standard Library, such as std::ifstream and std::unique_ptr, are movable but not copyable, and std::thread is one of them.
* 


#TBC: Ch 2.3
