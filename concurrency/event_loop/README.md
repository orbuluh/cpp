# [Idiomatic Event Loop in C++ by Tony Space](https://habr.com/en/post/665730/)
- Randomly found on the internet, nice article and code

# Quick fact
- Event Loop can be considered as an alternative for the mutex.
- Both of them serialize accesses to the guarded object, but in a slightly different manner.
- [Code](event_loop.h)
  - The implementation is quite compact. Feels idiomatic, doesn’t it? But 

# what is so special about it and what makes it efficient?

## The Power of `std::function`
- `std::function<R(Args...)>` uses two very important idioms that make it so useful for us – Type Erasure and Small-Object Optimization.
- Type Erasure idiom allows us to store anything that we can apply the call operator to, e.g. the `callable_t` in [code](event_loop.h).
- It can be a C-like function, it can be a functor. It can also be a lambda, including a generic lambda.

- Since our `callable_t` can have internal data, such as the functor’s members or the lambda’s capture, `std::function` also has to store all this data inside.
- In order to avoid or at least minimize heap allocations, `std::function` can store `callable_t` in place if it’s small enough.
  - What is “small enough” depends on the implementation. If it doesn’t fit into the internal storage, then heap allocation happens as a fallback.
  - This is what Small-Object Optimization essentially is.

- Knowing all of that, you can use a std::vector of std::function to keep both data and pointers to vtables in a single chunk of memory for most cases.
  - And this is the first member of our class – `std::vector<callable_t> m_writeBuffer;`

## The Power of `std::condition_variable`
- A condition variable is a synchronization primitive that makes one thread **postpone its execution via `wait()` until another thread wakes it up via `notify_one()`**.
- But what if the second thread has called `notify_one()` just before the first thread calls `wait()`? If you used a simpler synchronization primitive, such as Win32 Event Object, the first thread would not see the notification at all. In this case, it would fall asleep ending you up having your program deadlocked.
- Fortunately, `std::condition_variable` is a bit smarter.
  - It deliberately wants you to lock the mutex which guards some state.
  - While you’re holding the locked mutex `std::condition_variable` wants you to check the state if the first thread has to fall asleep or the condition has already been satisfied and the thread just needs to keep going.
  - If it turns out that the thread has to be postponed something interesting happens next.
  - Have you noticed that the `wait()` function accepts that locked mutex?
  - This is because the `wait()` function asks your operating system to do the following:
    - Atomically unlock the mutex and postpone the execution.
    - Atomically lock the mutex and resume the execution when `notify_one()` / `notify_all()` call occurs.

- I’m saying “atomically” here, but maybe it’s not what you’re thinking about.
- It’s a feature of the operating system thread scheduler, not the processors’ hardware.
  - **You cannot emulate that behavior using `atomic` variables.**
- Sometimes the OS may wake the waiting thread up spontaneously. It’s called “spurious wakeups”.
- So if the thread has been woken up, you need to check your condition again. That’s why I’m passing the predicate to the `wait()` function here.
- That version of `wait()` tests the condition before falling asleep and immediately after.

- As you can see we have to have a protected state guarded by the mutex. Therefore, the second notifying thread should do the following:
  - Lock the mutex, change the shared state, and unlock the mutex.
  - Notify the first thread.
- This is essentially what happens in the `enqueue()` function.
- Some developers call `notify_one()` while holding the mutex. It’s not wrong, but it makes the scheme inefficient.
  - In order to avoid extra synchronizations, just make sure you call the `notify_one()` after you release the mutex.

## The Power of Double Buffering
- What really makes this implementation especially efficient is that we have two buffers here.
- You may have noticed that we are swapping `readBuffer` and `m_writeBuffer`.
- And we are doing this while the mutex is being locked. `std::swap` simply swaps the pointers inside those two vectors, which is an extremely fast operation.
- So we are leaving `m_writeBuffer` empty, ready to be filled again. Next to the `std::swap` the scope ends unlocking the mutex.

- Now we have a situation where the write buffer is getting filled while the read buffer is being processed.
- Now, these two processes can go simultaneously without any intersection! When the processing is over, we clear the read buffer.
- **Clearing of `std::vector` does not cause the underlying storage deallocation.** So when we quickly swap those two buffers again this underlying storage is going to be filled up again as the write buffer.

- When you’re calling `enqueue()` and your `callable_t` is small enough you won’t even touch the heap while constructing it.
- Inserting this `callable_t` into the vector that has already got some storage left after the processing step takes almost nothing!
- What about locking the mutex? It turns out that modern mutexes use atomic spin-locks as the first step and after several iterations, they ask the operating system to postpone the thread.
  - So if you do something really quick between `lock()` and `unlock()` you won’t even disturb the operating system.
- `notify_one()` is also designed to be fast in case you call it while the mutex is unlocked. So you shouldn’t be bothered with it here. It makes `enqueue()` extremely fast on average. It also makes `wait()` fast as well, because we simply check if `m_writeBuffer` is not empty and swap it.

## Regarding "noexcept"
- You may be wondering, why am I using `noexcept` keywords all over the place. And there is a really good explanation in ISO C++ Core Guidelines why you should at least consider using it.
- Let’s take the `enqueue()` function. Even though we do have vector insertion here, which may throw `std::bad_alloc`, this scenario is actually disastrous.
- Your system either is lacking memory, which can cause a crash somewhere else, or you pushed too many tasks that your working thread cannot handle, which is in fact a poor application design.
- The lack of the system memory may also prevent throwing the exception about well ... the lack of the system memory since throw uses heap allocation.
- The same logic is applied to the destructor. I’m also enforcing `callable_t` to be passed by r-value reference to the `enqueue()` function.
- In this case, if the `callable_t` constructor fails it happens outside the Event Loop class.
- The situation with the thread function is different. Unfortunately, we cannot declare `callable_t` as `std::function<void() noexcept>()` enforcing the client to catch all the exceptions.
- So if the user’s code throws, we cannot properly handle it.
- I’m not sure that catching all exceptions by the event loop is a suitable strategy. I’d prefer to just automatically `std::terminate()` here. But it’s up to you to decide.

## `enqueueSync`
- If having the `enqueue()` function is not enough for you and you want to wait for the result - `enqueueSync` is the way to go.
- The first if-condition is a protection from a deadlock. **Sometimes you may discover a situation when some synchronous task is trying to schedule another synchronous task, leading to a deadlock.**
- I’m also using here `std::packaged_task` in conjunction with the `std::future`.
  - This is a nice way to transfer the function invocation result across the threads via `std::future`, including all of the exceptions that occurred.
  - Please note, that the `enqueueSync()` function is not declared as `noexcept` for this purpose.

## `enqueueAsync`
- Sometimes you may find it useful to obtain it for further usage instead of waiting for it immediately - you can use `enqueueAsync`
- Firstly, as you can see, there is no deadlock protection here, since it’s impossible to detect when and where the result is going to be used. So it’s up to the user to call the method properly.
- Secondly, we are using `std::shared_ptr` here. This is because we are bypassing the limitation of `std::packaged_task`, which is movable only. \
- However, `std::function` requires the underlying object to be copyable.
- And finally, we’re using here `std::bind` to copy or move all the arguments, because we are not aware of their lifetime.
- It’s a protection from dangling references. If you really want to pass an object by reference to enqueueAsync(), you can either capture it as [&] while defining lambda or using `std::ref()` or `std::cref()`.

