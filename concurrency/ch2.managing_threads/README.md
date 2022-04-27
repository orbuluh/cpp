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

#TBC: Ch 2.2