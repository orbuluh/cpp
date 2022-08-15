# Memory order
- Notes taken from
  - [modernescpp.com post 1](https://www.modernescpp.com/index.php/synchronization-and-ordering-constraints)
  - [modernescpp.com post 2](https://www.modernescpp.com/index.php/sequential-consistency-applied)
  - [modernescpp.com post 3](https://www.modernescpp.com/index.php/acquire-release-semantic)
  - [modernescpp.com post 4](https://www.modernescpp.com/index.php/transivity-of-the-acquire-release-semantic)
  - [modernescpp.com post 5](https://www.modernescpp.com/index.php/acquire-release-semantic-the-typical-misunderstanding)
  - [cppreference](https://en.cppreference.com/w/cpp/atomic/memory_order)

- You can not configure the atomicity of an [atomic data type](momory_model.md), but you can adjust very accurately the synchronization and ordering constraints of atomic operations.

# The six variants of the C++ memory model
- C++ has six variants of the memory model.
- The default for atomic operations is `std::memory_order_seq_cst`. But you can explicitly specify one of the other five
```cpp
enum memory_order{
  memory_order_relaxed,
  memory_order_consume,
  memory_order_acquire,
  memory_order_release,
  memory_order_acq_rel,
  memory_order_seq_cst
}
```
# Types of atomic operations
- For which type of atomic operations should you use the memory model?
- The memory model deals with reading and/or writing atomic operations.
  - read operation: `memory_order_acquire` and `memory_order_consume`
    - `is_lock_free`
    - `load`
  - write operation: `memory_order_release`
    - `clear`
    - `store`
  - read-modify-write operation: `memory_order_acq_rel` and `memory_order_seq_cst`
    - `test_and_set`
    - `exchange`
    - `compare_exchange_strong`, `compare_exchange_weak`
    - `fetch_add`, `+=`, `fetch-sub`, `-=`
    - `fetch_or`, `|=`, `fetch_and`, `&=`, `fetch_xor`, `^=`
    - `++`, `--`
- `memory_order_relaxed` defines no synchronization and ordering constraints. So it will not fit in this taxonomy.

# Synchronization/ordering constraints
- Which synchronization and ordering constraints are defined by the memory model?
  - **Sequential consistency**: `memory_order_seq_cst`
  - **Acquire-release**: `memory_order_consume`, `memory_order_acquire`, `memory_order_release` and `memory_order_acq_rel`
  - **Relaxed**: `memory_order_relaxed`
- While the sequential consistency establishes a global order between threads, the acquire-release semantic establishes an ordering between reading and write operations **on the same atomic variable on different threads**.
- The relaxed semantic only guarantees that **operations on the same atomic data type in the same thread can not be reordered.** That guarantee is called **modification order consistency**. But **other threads can see this operation in a different order.**

# Sequential consistency example
- Check: [memory_order::demo()](demo/memory_order.h)
- program will always return "done" when the memory order is `memory_order_seq_cst`
- In producer:
  - `work= "done"` is **sequenced-before** `ready=true` / `work= "done"` **happens-before** `ready=true`
- In consumer:
  - `while(!ready.load()){}` is **sequenced-before** `std::cout<< work ...`, / `while(!ready.load()){}` is **happens-before** `std::cout<< work ...`.
- Producer and consumer is synchronized by the atomic variable `ready`
  - `ready = true` synchronizes-with `while(!ready.load()){}`
  - `ready = true` **inter-thread happens-before** `while (!ready.load()){}`
  - `ready = true` **happens-before** `while (!ready.load()){}`

# From the sequential consistency to the acquire-release semantic
- Check [memory_order::demo()](demo/memory_order.h) - run few times and you will see.
- When using `memory_order_seq_cst`, a thread sees the operations of another thread and therefore of all other threads in the same order.
- If we use the acquire-release semantic for atomic operations, the key characteristic of the sequential consistency will not hold.
  - There is no global synchronization between threads in the acquire-release semantic, there is **only a synchronization between atomic operations on the same atomic variable.**
  - So a **write** operation on one thread **synchronizes** with a **read** operation on another thread **on the same atomic variable**.
  - This synchronization relation on the same atomic variable helps to establish a **happens-before** relation **between atomic variables and therefore between threads.**
  - e.g. In acquire-release semantic, we will not reason about the synchronization of threads. We have to reason about the synchronization **of the same atomic in different threads**.

# Synchronization and ordering constraints
- The acquire-release semantic is based on one key idea.
  - A *release* operation synchronizes with an *acquire* operation on the same atomic and establishes, in addition, an ordering constraint.
  - So, **all read and write operations** can **not** be moved **before** an *acquire* operation, all read and write operations can **not** be move **behind** a *release* operation.
- But what is an acquire or release operation?
  - The reading of an atomic variable with `load` or `test_and_set` is an *acquire* operation. It's like acquiring/locking of a lock.
  - Accordingly, a `store` or `clear` operation on an atomic variable is a *release* operation. It's like a releasing/unlocking of a lock.
- That implies, that
  - **an operation on a variable can not moved outside of a critical section. That hold for both directions.**
  - a variable **can** be moved **inside** of a critical section. Because the variable moves from the not protected to the protected area.


- A *release* operation **synchronizes** with an *acquire* operation on **the same atomic variable** and establishes, in addition, ordering constraints.
- Check [memory_order_sync_with_atomic::demo](demo/memory_order.h)
- These are the components to synchronize threads in a performant way, in case they **act** on the same atomic.
- How? Because the transitivity of the acquire-release semantic, threads can be synchronized, which act independently of each other.
```cpp
void dataProducer() {
    taskQ.push(10);                                              // 1
    dataProduced.store(true, std::memory_order::release);        // 2
}

void broker() {
    while (!dataProduced.load(std::memory_order::acquire)) {;}   // 3
    dataConsumed.store(true, std::memory_order::release);        // 4
}

void dataConsumer() {
    while (!dataConsumed.load(std::memory_order::acquire)) {;}   // 5
    std::cout << taskQ.front() << '\n';                          // 6
    taskQ.pop();
}
```
- (1, 2) / (3, 4) / (5, 6) has the **sequenced-before** relations.
  - That means, that all operations in one thread will be executed in source code order
  - 1 before 2, 3 before 4, 5 before 6
- (2, 3), (4, 5) are the **synchronize-with** relations.
  - The reason is the **acquire-release semantic of the atomic operations on the same atomic**.
  - So the synchronization between the threads takes place.
  - e.g. because the sync at (2, 3) - 4 cannot happen before 1,
    - all read and write operations can **not** be move **after** a *release* operation. (1 wont happen after 2)
  - similarly, because the sync at (4, 5) - and 6 can not happen before 3
    - all read and write operations can **not** be moved **before** an *acquire* operation. (4 won't happen before 3)

# Closer look
- Compare below 2 snippets, step 3 in second snippet doesn't have the while loop
  - Snippet will segfault - because although 2 should sync with 3, there is no guarantee that 2 will happen before 3.
  - So the purpose of the while loop in 3 of snippet 1 is basically making sure that `dataProduced.store(true, std::memory_order::release)` happens before `dataProduced.load(std::memory_order::acquire)` evaluate to true.
```cpp
// OK example
void dataProducer() {
    taskQ.push(10);                                              // 1
    dataProduced.store(true, std::memory_order::release);        // 2
}

void dataConsumer() {
    while (!dataProduced.load(std::memory_order::acquire)) {;}   // 3
    taskQ.pop();                                                 // 4
}
```
```cpp
// Bad!
void dataProducer() {
    taskQ.push(10);                                              // 1
    dataProduced.store(true, std::memory_order::release);        // 2
}

void dataConsumer() {
    dataProduced.load(std::memory_order::acquire);               // 3
    taskQ.pop();                                                 // 4
}
```




# Quick facts
- From [C++ High Performance...](https://www.amazon.com/High-Performance-Master-optimizing-functioning/dp/1839216549)
- When it comes to shared variables in multi-threaded programs, we need to be aware of instruction re-orderings.
- The compiler (and hardware) does all its optimizations based on what is true and observable **for one thread only**.
- The compiler cannot know what other threads are able to observe through shared variables, so it is our job as programmers to inform the compiler of what re-orderings are allowed.
- When protecting a critical section with a mutex, it is guaranteed that only the thread that currently owns the lock can execute the critical section.
  - But, the **mutex is also creating memory fences around the critical section to inform the system that certain re-orderings are not allowed at the critical section boundaries**.
  - When **acquiring the lock, an acquire fence is added, and when releasing the lock, a release fence is added.**

# Herb Sutter - atomic Weapons
- [part 1](https://youtu.be/A8eCGOqgvH4) / []()

# [Arvid Norberg: The C++ memory model: an intuition](https://youtu.be/OyNG4qiWnmU)
- About considering sharing...
  - 1. aim for "no sharing"
  - 2. mutex often performs really well
  - 3. atomic operations is the last resort, but be aware that it may perform worse. So measure!
- Atomic operations are all about
  - synchronizing memory between threads
  - avoiding torn load/stores and clobbering updates
- Synchronizing threads mean all regular memory (non atomic writes) is
  - made available from our thread to other threads
  - from other threads to our thread
- If you have a data race, everything goes out of window

- atomic has different "mode" ... e.g. memory order
  - sequentially consistent
  - acquire
  - release
  - relaxed
  - consumed
- why acquire/release? Think it like a mutex:
  - mutex.lock() ~= acquire
  - mutex.unlock() ~= release
- Other than mutual exclusion, what happen when you acquire a mutex and release a mutex?
  - When you **release**, you "publish" all the memory you've updated during the critical sections. Those changes become available to other threads.
  - When you **acquire**, it's like you pulled in the memory published/updated by other threads, e.g. other threads has to release their updates.
```cpp
mutex.lock() // acquire updates from other thread
//-----
// store to memory, updates not available to other threads during critical sections
//-----
mutex.unlock() // release updates from this thread to other thread
// memory updates NOT available to other threads until they call lock() / acquire the updates
```




# `std::memory_order` Quick fact

- `std::memory_order` specifies how memory accesses, **including regular, non-atomic memory accesses, are to be ordered around an atomic operation**.
- Absent any constraints on a multi-core system, when multiple threads simultaneously read and write to several variables, one thread can observe the values change in an order different from the order another thread wrote them.
  - Indeed, the apparent order of changes can even differ among multiple reader threads.
  - Some similar effects can occur even on uniprocessor systems due to compiler transformations allowed by the memory model.

- The default behavior of **all atomic operations** in the library provides for **sequentially consistent ordering** (see discussion below).
  - That default can hurt performance, but the library's atomic operations can be given an additional `std::memory_order` argument to specify the exact constraints, beyond atomicity, that the compiler and processor must enforce for that operation.

# Formal description
- Inter-thread synchronization and memory ordering determine how **evaluations and side effects of expressions are ordered between different threads of execution**. They are defined in the following terms:

##  Modification order
- **All modifications** to any **particular** atomic variable **occur in a total order** that is specific to this one atomic variable.
- The following four requirements are guaranteed for all atomic operations:
  - 1) **Write-write coherence**: If evaluation A that modifies some atomic `M` (a write) happens-before evaluation B that modifies `M`, then A appears earlier than B in the modification order of `M`
  - 2) **Read-read coherence**: if a value computation A of some atomic `M` (a read) happens-before a value computation B on `M`, and if the value of A comes from a write X on `M`, then the value of B is either the value stored by `X`, or the value stored by a side effect Y on `M` that appears later than X in the modification order of `M`.
  - 3) **Read-write coherence**: if a value computation A of some atomic `M` (a read) happens-before an operation B on `M` (a write), then the value of A comes from a side-effect (a write) X that appears earlier than B in the modification order of `M`
  - 4) **Write-read coherence**: if a side effect (a write) X on an atomic object `M` happens-before a value computation (a read) B of `M`, then the evaluation B shall take its value from X or from a side effect Y that follows X in the modification order of `M`

## Carries dependency
- Within the **same thread**, evaluation A that is sequenced-before evaluation B **may also carry a dependency** into B (that is, **B depends on A**), if any of the following is true
  - 1) The value of A is used as an operand of B, except
    - a) if B is a call to std::kill_dependency
    - b) if A is the left operand of the built-in `&&,` `||`, `?:`, or `,` operators.
  - 2) A writes to a scalar object M, B reads from M
  - 3) A carries dependency into another evaluation X, and X carries dependency into B

##  Release sequence
- After a release operation A is performed on an atomic object `M`, the longest **continuous subsequence of the modification order** of `M` that consists of
  - 1) Writes performed by the same thread that performed A (until C++20)
  - 2) Atomic read-modify-write operations made to `M` by any thread
- is known as release sequence headed by A

## Sequenced-before
- Within the **same thread,** evaluation A **may** be sequenced-before evaluation B, as described in evaluation order.

## Dependency-ordered before
- **Between threads**, evaluation A is dependency-ordered before evaluation B **if any** of the following is true
  - 1) A performs a release operation on some atomic `M`, and, in a different thread, B performs a consume operation on the same atomic `M`, and B reads a value written by any part of the release sequence headed (until C++20) by A.
  - 2) A is dependency-ordered before X and X carries a dependency into B.

## Inter-thread happens-before
- **Between threads**, evaluation A inter-thread happens **before** evaluation B **if any** of the following is true
  - 1) A synchronizes-with B
  - 2) A is dependency-ordered before B
  - 3) A synchronizes-with some evaluation X, and X is sequenced-before B
  - 4) A is sequenced-before some evaluation X, and X inter-thread happens-before B
  - 5) A inter-thread happens-before some evaluation X, and X inter-thread happens-before B

## Happens-before
- Regardless of threads, evaluation A happens-before evaluation B **if any** of the following is true:
  - 1) A is sequenced-before B
  - 2) A inter-thread happens before B
- The implementation is required to ensure that the happens-before relation is acyclic, by introducing additional synchronization if necessary (it can only be necessary if a consume operation is involved)
- If one evaluation modifies a memory location, and the other reads or modifies the same memory location, and if at least one of the evaluations is not an atomic operation, the behavior of the program is undefined (the program has a data race) unless there exists a happens-before relationship between these two evaluations.

## Simply happens-before
- Regardless of threads, evaluation A simply happens-before evaluation B **if any** of the following is true:
- (since C++20)
  - 1) A is sequenced-before B
  - 2) A synchronizes-with B
  - 3) A simply happens-before X, and X simply happens-before B
- Note: without consume operations, simply happens-before and happens-before relations are the same.

## Strongly happens-before
- Regardless of threads, evaluation A strongly happens-before evaluation B **if any** of the following is true:
- (until C++20)
  - 1) A is sequenced-before B
  - 2) A synchronizes-with B
  - 3) A strongly happens-before X, and X strongly happens-before B
- (since C++20)
  - 1) A is sequenced-before B
  - 2) A synchronizes with B, and both A and B are sequentially consistent atomic operations
  - 3) A is sequenced-before X, X simply happens-before Y, and Y is sequenced-before B
  - 4) A strongly happens-before X, and X strongly happens-before B
- Note: informally, if A strongly happens-before B, then A appears to be evaluated before B in all contexts.
- Note: strongly happens-before excludes consume operations.

# Visible side-effects
- The side-effect A on a scalar `M` (a write) is visible with respect to value computation B on `M` (a read) if **both of the following** are true:
  - 1) A happens-before B
  - 2) There is no other side effect X to **M** where A happens-before X and X happens-before B
- If side-effect A is visible with respect to the value computation B, then the **longest contiguous subset of the side-effects** to `M`, in modification order, where B does not happen-before it is known as the **visible sequence of side-effects**.
- (the value of `M`, determined by B, will be the value stored by one of these side effects)
- Note: inter-thread synchronization boils down to preventing data races (by establishing happens-before relationships) and defining which side effects become visible under what conditions

### Consume operation
- Atomic load with `memory_order_consume` or stronger is a consume operation.
- Note that std::`atomic_thread_fence` imposes stronger synchronization requirements than a consume operation.

### Acquire operation
- Atomic load with `memory_order_acquire` or stronger is an acquire operation.
- The `lock()` operation on a Mutex is also an acquire operation.
- Note that `std::atomic_thread_fence` imposes stronger synchronization requirements than an acquire operation.

### Release operation
- Atomic store with `memory_order_release` or stronger is a release operation.
- The `unlock()` operation on a Mutex is also a release operation.
- Note that `std::atomic_thread_fence` imposes stronger synchronization requirements than a release operation.


### `memory_order_relaxed`
- Relaxed operation: there are no synchronization or ordering constraints imposed on other reads or writes, only this operation's atomicity is guaranteed.
- Atomic operations tagged `memory_order_relaxed` are **not synchronization operations**; they **do not impose an order among concurrent memory accesses**.
- They **only guarantee atomicity and modification order consistency.**

For example, with x and y initially zero,

// Thread 1:
r1 = y.load(std::memory_order_relaxed); // A
x.store(r1, std::memory_order_relaxed); // B
// Thread 2:
r2 = x.load(std::memory_order_relaxed); // C
y.store(42, std::memory_order_relaxed); // D
is allowed to produce r1 == r2 == 42 because, although A is sequenced-before B within thread 1 and C is sequenced before D within thread 2, nothing prevents D from appearing before A in the modification order of y, and B from appearing before C in the modification order of x. The side-effect of D on y could be visible to the load A in thread 1 while the side effect of B on x could be visible to the load C in thread 2. In particular, this may occur if D is completed before C in thread 2, either due to compiler reordering or at runtime.

Even with relaxed memory model, out-of-thin-air values are not allowed to circularly depend on their own computations, for example, with x and y initially zero,

// Thread 1:
r1 = y.load(std::memory_order_relaxed);
if (r1 == 42) x.store(r1, std::memory_order_relaxed);
// Thread 2:
r2 = x.load(std::memory_order_relaxed);
if (r2 == 42) y.store(42, std::memory_order_relaxed);
is not allowed to produce r1 == r2 == 42 since the store of 42 to y is only possible if the store to x stores 42, which circularly depends on the store to y storing 42. Note that until C++14, this was technically allowed by the specification, but not recommended for implementors.

(since C++14)
Typical use for relaxed memory ordering is incrementing counters, such as the reference counters of std::shared_ptr, since this only requires atomicity, but not ordering or synchronization (note that decrementing the shared_ptr counters requires acquire-release synchronization with the destructor)

