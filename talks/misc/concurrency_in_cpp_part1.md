# Concurrency in C++: A Programmer’s Overview (part 1 of 2) - Fedor Pikus


## What has been in standard?

**Pre-C++11**:

- (NOTHING): The standard does not mention thread nor concurrency

**C++11**

- Thread - both the notion and the object
- Memory model
- Concurrency primitives - mutex, condition ...etc
- Promises and futures
- Atomic values and operations
- Memory barriers and memory order
- Thread-local data
- Thread safety guarantees

**C++14**

- Shared lock
- Shared timed mutex

**C++17**

- Cache line size
- Scoped lock
- Shared mutex
- Parallel STL and execution policies

**C++20**

- Auto-join thread
- Thread cancellation
- Atomic operations for non-atomic objects
- Atomic wait and notification
- Atomic flags
- More concurrency primitives (semaphores, latches, barriers)
- Unsequenced execution policy
- Coroutines


## We did have concurrent program before C++11, but how/why?

Question is, why they (sometimes) worked, and what was really missing before C++11?

```cpp
// C++03 concurrency code
void do_work(double* x, pthread_mutex_t* mutex) {
  for (size_t i = 0; i < 10; ++i) {
    pthread_mutex_lock(mutex);
    ++x[i];
    pthread_mutex_unlock(mutex);
  }
}
```

- The issue is - `pthread_mutex_*lock` only operates on `mutex`, and it is completely independent to `x` and `i`.
- Because of this, "hoisting common sub-expressions" is what compilers do to optimize these independent sequence:

```cpp
// Potential compiler optimization through hoisting common sub-expressions
void do_work(double* x, pthread_mutex_t* mutex) {
  pthread_mutex_lock(mutex); // operates only on mutex
  pthread_mutex_unlock(mutex); // operates only on mutex
  for (size_t i = 0; i < 10; ++i) {
    ++x[i];  // operates only on x
  }
}
```

- According to standard, this is a legitimate optimization, and nothing from C++03 prevent compiler from doing this. But if this does happen, the code would have not worked. Why does this work in practice?
- Say we have some magical thing happen to make this work. Another question: Is this thread-safe pre C++11?

```cpp
int data[N];
do_work(data[0]); // executing on thread 1
do_work(data[1]); // executing on thread 2
```

- Taking it further, say we not just have `int`, we have any type `T`, is it thread-safe (note: standard has not even "thread-safe" wording back then.)/worked as expected?


## The magic that makes pre-C++11 concurrency work

Maig: Code reordering is allowed by C++ but controlled by POSIX

- Without POSIX there is no `pthread_mutex_t` to use
- Every compiler that was allowed to write concurrent program support both C++ and POSIX standards and abide by combined restrictions.
- So basically when enabled, it is a global side effect saying you can't reorder around the `pthread_*`
- One issue however, is this is not native to C++ itself. C++ should have its own memory model, which doesn't need to be exactly the same as POSIX.
- The other issue, is the `T data[N]` case - in POSIX, if T is `int`, it's doing okay. If `T` is `bool` (pretty easy to observe the race if it's bool), you are out of luck. And if `T` is short, you may or may not do what you expect.
- Or more general, concurrent data access is not specified by POSIX (nor pre-C++11), so anything can/did happen, generally not safe for `bool` arrays, and may be unsafe for `int` and other types depending on generated code.
- In general, POSIX memory model does not provide all necessary guarantees.

## What is missing? The memory model

> **What is memory model?**: memory model describes the interaction of threads through memory, their use of the shared data, and its effect on concurrent applications.

- We will discuss memory model in details in the 2nd part. Let's check some practical tools first.

## C++ 11 `std::thread`s

C++ thread is a type-erased non-polymorphic class

- Any callable returning `void` is allowed. (Lambdas are often used.)
  - A new thread is created and the callable is invoked
- Constructor is a function template but the class does not depend on constructor argument types (e.g. type erasure)
- Usually done with run-time polymorphism inside the class
- Thread must be explicitly joined (join blocks until the work is done.)
  - If you destroyed thread without join'ed, it's undefined behavior
- Thread cannot be reused but the main work function can ask for more work.
- Check code [example](../../concurrency/demo/jthread_example.h)
- In practice, launching a thread for every computation makes the program slower or much slower.

## What are threads for?

- Threads are usually created for performance - concurrent execution of code takes advantage of hardware resources (multiple CPUs)
- Often threads are created for asynchronous execution: UI, I/O, network
  - Sometimes you just don't want to be blocked, not performance related per-se.
  - Sometimes it's easier to create a thread than to track multiple events.

## `std::thread` is a user thread not kernel thread

**Kernel threads** are independent instruction sequences (from the point view of processor) as seen by the OS and the hardware.

- OS schedules kernel threads to run on the CPU. (You can have more kernel thread than CPU)
- At any time, at most one kernel thread per processing core can run (what is a core?)
  - (Or say, one CPU can only run execution sequence from one kernel thread)

Creating a new `std::thread` gives you a user thread (as you can imagine, you created it in user space)

OS executes all user threads on however many kernel thread it runs

- Often, user threads correspond to kernel threads (1:1 model, windows, Linux all this model)
- Sometimes, the OS runs N user threads on M kernel threads (N:M model)
  - N is what user requested, M is whatever the OS wants
  - User threads are like "tasks" executed by the "pool" of kernel threads
  - In N:M model, it's possible that user create super large N threads, and OS will handle it. But it's hard to get right and general OS doesn't use this model.

<details><summary markdown="span">Recap: Kernel/User space (Generated by ChatGPT)</summary>

In computer systems, kernel space and user space are two distinct memory regions that operate at **different privilege levels.**

- The **kernel** has complete control over the system, including access to memory and I/O devices. It is responsible for managing system resources and providing a platform for other programs to run on.

> **Kernel space** is the **memory area where the operating system kernel and device drivers are executed.**

- It is a privileged memory area that has **direct access sto all hardware resources and system-level functions.**

> **User space**, on the other hand, is the memory area where user programs and applications are executed.

- It operates at a lower privilege level than kernel space and does not have direct access to hardware resources or system-level functions. Instead, **it relies on the kernel to provide access to these resources and functions through system calls and other mechanisms.**

In general, kernel space is reserved for the operating system and system-level functions, while user space is reserved for user programs and applications. This separation of privileges helps to protect the system from malicious or faulty user programs, as they cannot directly access or manipulate critical system resources.
</details>


## Why not 1 million threads?

- Large number of compute-intensive kernel threads competing for much fewer processors create overhead
  - `std::thread` is a user thread not kernel thread, but most common OS only offers 1:1 thread model

> :brain: In practice, launching a thread for every computation makes the program slower or much slower

## Threads are usually expensive to start and join

- Expensive? please measure: [here it is](../../concurrency/demo/thread_is_heavy.h)
- Thread creation throughput and latency are generally tricky to measure, maybe 10-100ms aor much less (got reused)
- Individual operations are in nano second range, but start/join is in micro seconds range, so hundred of thousands operations
- Tail latency: delay from completing the thread to completing join (usually few milliseconds)
- Throughput
  - For independent workloads, N times faster until N ~CPU count
  - Memory and I/O bound workloads scale less
  - Any data exchange between threads reduces throughput
  - SMT (hyperthreading) cores are not real cores
  - Some hardware resources do not scale with core count.

## How do we use threads correctly?

threads are expensive to start and join, so we have to **manage work on thread ourselves**

- Create ~1 compute thread per CPU core (maybe counting SMT cores)
- Keep them alive for a long time (so no need to pay the overhead for start/join)
- Give them a piece of work (a.k.a task) as it becomes available
- Idle them when there is no work to do

Background/idel threads are much easier:

- OS manages them well
- You can create many such threads with no problems
- For example: one thread per socket connection or one UI thread per remote user.


## Futures and promises

- `std::async`: execute an action (callable) eventually
  - returns a `std::future`
- `std::future`: a placeholder for a future result
  - result is computed asynchronously
  - the caller can wait for it (blocking or non-blocking)
- `std::promise`: a temporary holder of future results
  - eventually becomes a future
  - supports "abandon" - often to deliver exceptions from asynchronous jobs

## Don't use `std::async` ...

`std::async` (in theory) is all you need, but in practice, it's entirely useless if the aim is performance.

- Also no good for large number of small tasks
- May be ok for long-term background tasks.

The problem is in the implementation

- The standard imposes very few restrictions on how the work is actually done. Standard does not strictly require a pool implementation but doesn't forbid one either.
- Two options from standard about the implementation: serially (then you don't get scale) or concurrently (e.g. just fire a thread, which most implementation does)

What we need is a thread scheduler (executor, in C++ dialect)

- Wait at least until C++17 or until C++23


## What about `std::future` and `std::promises`
















