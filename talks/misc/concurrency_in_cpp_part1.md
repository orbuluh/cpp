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

- Most threading libraries (thread pool, work queue, ...etc) have their own facilities to wait on results. (Sometimes, they look a lot like `std::future`)

## Where there are threads, there must be locks

- C++ has `std::mutex`, a basic lock that usually implemented on top of the OS lock like `pthread_mutex_t`
- Has `lock()`, `unlock()` and `trylock()` methods, but use RAII for mutexes through

```cpp
void thread_worker(int i) {
  std::lock_guard guard(my_mutex);
  sum += 1;
}
```

Ownership for mutexes: `std::unique_lock`

- Can be used like `lock_guard`
- Can be moved (ownership transfer)
- Works well with `std::lock()` - the non-RAII way to lock several mutexes at once.
- Less interesting since `std::scoped_lock` in C++17

## Where there are mutexes, there are deadlocks

**:lock: Guaranteed deadlock: second lock on the same thread**

```cpp
std::mutex m;
m.lock();
m.lock();
```

- Preferable to avoid this.
- Sometimes used for class members calling other class members, better to use private non-locking member variants. (e.g. separate those logic into a private function that has no lock, while you can assume when it's used, it's locked.)
- Or you can do - recursive mutex - `std::recursive_mutex`

- Recursive mutex - `std::recursive_mutex`

**:lock_with_ink_pen: Likely deadlock**

Both thread are trying to get the lock for the lock holding by other thread

```cpp
std::mutex m1, m2, m3;

void thread_worker1(int i) {
  m1.lock(); // possible: hold m1, wait m2
  m2.lock();
}

void thread_worker2(int i) {
  m2.lock(); // possible: hold m2, wait m1
  m1.lock();
}
```

- General proble: mutexes are not composable
- `std::lock(m1, m2, m3, ...)` guarantees no deadlock
- Often uses with std::unique_lock to unlock automatically
- Or C++17 RAII `std::scoped_lock l(m1, m2, m3)` is like `lock_guard` + `lock()`

## C++17 Read-write locks: `std::shared_mutex`

(Nice in theory, but in practice, performance is usually bad - measure before using it)

`lock()`: exclusive lock for write access

- RAII wrapper for exclusive locking, use `std::unique_lock` like before

`lock_shared()` shared lock for read access

- RAII wrapper for shared locking, use `std::shared_lock` (since C++14)
- Note `lock()` on `std::shared_lock` calls `lock_shared()` underneath

Note: terminology deviation from POSIX: this is POSIX read-write mutex, while "POSIX shared mutex" is a cross-process mutex for shared memory

## Condition variables

- Condition is a synchronization barrier to block thread[s] until a signal to proceed is given (by another thread)
- Condition is always paired with a mutex.
- Key thing is when condition variable is waiting (for the signal from notify), it's not holding the lock. So it won't be blocked waiting to get the lock per-se - but blocked waiting to be signal. And then right after signal comes and predicate passes, it will lock the mutex automatically again.
- A `std::unique_lock` is needed if it is used with `std::condition_variable`. `std::lock_guard` may have less overhead, but it cannot be used with `std::condition_variable`, because any call to `std::condition_variable::wait()` on a will always need to `lock()` and `unlock()` the underlying mutex.

```cpp
//simple example to transfer data from one thread to another:

std::conditional_variable c;
std::mutex m;

void producer() {
  Data* data = nullptr;
  {
    std::lock_guard l(m);
    data = new Data;
  }
  c.notify_all(); //signal the condition
}

void consumer() {
  // note:: need to pass unique_lock so wait can call the unlock for wait
  std::unique_lock l(m);
  // hold the mutex first, then unlocks mutex while wait
  // wake up and check predicate when receive signal
  c.wait(l, [&] { return bool(data); });
  //-----re-locks m when predicate is true and no longer wait
  // (so if some other thread is locking the mutex right at this points, now it
  // will wait to get the access of mutex and is blocked here.)
  do_work(data);  // guarded by m, and when it goes out of scope, the unique_lock destruct and unlock.
}
```

## Timeouts: `std::timed_mutex` and timed variants of other mutexes

- `lock()` is as usual blocking
- `try_lock()` is as usual, non-blocking
- `try_lock_for()` wait for the specified time to lock the mutex
  - returns true if the mutex was locked
  - false does not mena the mutex is locked by another thread (though usually is)
- `std::condition_variable` has `wait_for()`, which returns status and mutex is always re-acquired when waiting ends (even if it's because timeout)

## C++11 static initialization

Local static variables are guaranteed to be thread-safe

- Usually implemented efficiently with double-checking locking

```cpp
// this is guaranteed to be good even if it's called concurrently
Singleton& f() {
  static Singleton* const p = new Singleton;
  return *p;
}
```

```cpp
std::once_flag done; // global
std::Call_once(done, []{ /*do sth...*/});
```

- `std::call_once()` - equivalent of `pthread_once`
- Concurrent calls are safe, only one (active call) is executed.
- If exception is thrown, call fails (does not count)

## `thread_local` variables / storage type

- It's like `static`, but one copy per thread. (`static` is one per process)
- Storage duration is as long as the thread exists
- Constructors and destructors of the variable run when the thread is created/joined
- Every new thread creates a new instance of the object
- Can be used with or without static but affects only linkage.
- Performance wise it should be the same as single thread access static local variable.
- Check code [example](../../concurrency/demo/thread_local.h)

```cpp
thread_local int i = 0;
int main() {
  std::cout << &i << '\n'; //addr1
  std::jthread t([&]{ std::cout << &i << '\n'; }); //addr2 != addr1
}
```

## Barriers and Latches

`std::latch` used to synchronize thread arrival

```cpp
std::latch sync(2);
std::thread t1([&] { sync.arrive_and_wait(); });
std::thread t2([&] { sync.arrive_and_wait(); });
```

- You declare latch with a count
- All threads are held at the latch until the counter drops to 0
- Latch can not be reused.

`std::barrier` is a reusable latch (or say, latch is a single-use barrier)

- Barrier countdown is reset (automatically) after all threads arrive
- Check code [example](../../concurrency/demo/barrier_example.h)


## Semaphores

Synchronization primitive for managing shared resources

- contains a count of available resources
- maximum count must be defined at compile time

- `std::counting_semaphore<max_count>::release()`: notify waiting threads that a resource is available, increment count
- `std::counting_semaphore<max_count>::acquire()`: wait until a resource is available, then decrement count

- When you acquire semaphore, you will be locked. When somebody else release semaphore, one of the thread (arbitrary chosen) that acquired the semaphore will be unblocked and allowed to proceed. Other acquired thread will still be blocked. If you call twice release, then 2 threads will be waken up. Each has the guarantee that there is enough resources for them. If someone release, and no one has acquired, it will just bump the count.

- This is usually used for resource transfer, for example, shared queue:
  - `acquire()` to pop a value or wait for one
  - `release()` to push a value

## Take a deeper look, C++11 memory model

**Why do we need a memory model? What is a memory model? Why does it matter?**

- Memory model describes interaction of threads through memory
  - A thread is a sequence of instructions that can be executed independently of other threads
- Memory model describes what **state of memory** can and cannot be observed by a thread in presence of other threads.
- In particular, memory model describes what guarantees we have when multiple threads are accessing memory.
- Compiler, OS, and hardware must provide certain guarantees to enable writing multi-threaded programs.
- The set of these guarantees forms the memory model.
- C++11 gave us the "C++ memory model".

## C++ memory model

C++ memory model: concurrent accesses to different memory locations are independent.

```cpp

T data[N];
data[0] = 0; // executing on thread 1
data[1] = 1; // executing on thread 2
```

- C++ memory model requires this to be safe for all types T (even bool)
  - Note: Hardware operates on words, not bytes nor bits. Hence pre-C++11, without such guarantee, this might not be safe. Because, say word is 4 bytes long. And you want to change specific byte. Hardware will just load the work with 4 bytes in, change that one byte, then write 4 bytes back. However, if some other thread comes in and change one of the other 3 bytes in the word, either you or other thread would have undo work from others.
- Given the available hardware features, the compiler must emit code that does not entangle different memory locations (This may come at the cost of some performance)
- Memory location is bytes, not bits. E.g. **NO guarantees about accessing separate bits in the same value.**

- Concurrent accesses to different memory locations are safe.
- Concurrent accesses to the same memory location are safe only if all accesses are reads (do not change value)
- :rotate_light: **Concurrent write[s] and read[s] to the same memory location, then for every thread, it's UB (not just the write thread or read thread)**
- **To avoid UB, accesses to the same variable must be synchronized**
  - mutexes, atomics, other synchronization options
  - **the point is to ensure that the accesses are not really concurrent.**


## C++ thread synchronization

From threads, came data races, from data races, came locks
```cpp
int i = 0;
std::mutex m;
{ std::lock_guard l(m); ++i; } // executed on thread 1
{ std::lock_guard l(m); ++i; } // executed on thread 2
```

- Mutex ensures exclusive access - only one thread can lock it at any time.
- When you lock it, only one thread has access to i at any time, so no more UB.
- What about the mutex itself? 2 Threads access it for locking (can't be read operation to "lock") at once! --> It's magic that we will talk later.

## Back to memory model

Memory model guarantees apply to both correctness and efficiency - the question is: What is guaranteed to work and what can be optimized?

**What is C++ memory model consists of?**

1. Memory ordering: what (if any) guarantees are given for relative order of memory reads and writes?
2. Synchronization events: at which points is memory access synchronized?
3. Consistency model: what memory state is guaranteed to be seen the same by multiple threads?
4. Memory visibility: when do memory writes become visible to other threads? (Closely related to memory ordering)

**What determines memory mode?**

- Hardware has a memory model (And/or virtual machine, if you use one)
- Language also defines a memory model
- Compiler may provide additional guarantees
- Compilers avoid transformations (including optimizations) that would violate the memory model of the language
- Compilers must use available hardware features to guarantee the memory model defined by the language.
- Memory model is a property of the entire system: compiler, runtime support and hardware.
- Hardware has a particular memory model
- Compiler can reorder instructions that would not be reordered by a given hardware
- Runtime system may enforce guarantees not normally provided by the hardware.

## What is memory order?

Different hardware platforms provide different order guarantees (memory model)

- Read-read  ... can 2 reads be reordered?
- Read-write
- Write-read
- Write-write
- Dependent reads (int* p; can read of p and read of *p be reordered?)

Great variety of scenarios for ordering:

- Read-atomic read
- Write-atomic write
- Read-atomic write
- Write-instruction fetch (prevents self-modifying code.)

## Different processors have different memory models

- X86 has a rather strict memory model:
  - Most operations are executed in program order
  - Write-read can be reordered as read-write (To clarify: compiler might have its own reordering as compared to the program ordering. But here, we are saying what hardware could do. E.g. hardware could has its own reordering as compared to compiler optimized ordering.)
  - Incoherent instruction cache pipelining (write-instruction fetch can be reordered)

- ARM has a very relaxed memory model:
  - Most operations can be arbitrarily reordered
  - Dependencies are respected (p is read before *p)
  - Only atomic operations and other special hardware instructions restrict memory order.

## Memory model from all sides

- Memory model describes interaction of threads and processes through shared memory
- Memory model specifies the complete set of guarantees for the order of memory operations. Including atomic operations, locks, ...etc
- More precisely, guarantees are given on **visibility** of memory operations.
  - So saying "memory model defined which memory accesses can and can not be reordered" is a simple model, what is more precise is "can be observed as reordered"
  - E.g. The effects of operations can be observed in different order.
  - So **:brain: "write-read can be reordered" - what happened is that write-read is still executed in write-read order. But the result of write maybe only become visible after the read, that's what actually happened when we say "write-read is reordered".**
- Memory model describes which parts of memory are guaranteed to be seen consistently by threads given a sequence of memory accesses by each thread.
- This is often referred to as consistency model - memory models may also limit allowed inconsistency.
- For practical purpose, it's just 2 different views that have equivalent result. So just think it with whatever feels more easy to you.

## So lock work anyway. Why do we care about memory model?

Because as a programmer, you can take explicit control over the guarantee that you want or not - only if you know the memory model.
