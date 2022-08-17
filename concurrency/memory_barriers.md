# Memory barriers
- Notes taken from
  - [preshing.com post 1](https://preshing.com/20120710/memory-barriers-are-like-source-control-operations/)
  - [modernescpp.com post 1](https://www.modernescpp.com/index.php/fences-as-memory-barriers)
  - [modernescpp.com post 2](https://www.modernescpp.com/index.php/acquire-release-fences)

# Memory Barriers Are Like Source Control Operations
- memory reordering could happen at compile time, and it could reordering at runtime, on the processor itself
- Like compiler reordering, processor reordering is invisible to a single-threaded program. **It only becomes apparent when lock-free techniques are used**
  - that is, when **shared memory is manipulated without any mutual exclusion between threads.**
  - However, unlike compiler reordering, the effects of processor reordering are only visible in multicore and multiprocessor systems.
- You can **enforce correct memory ordering on the processor by issuing any instruction which acts as a memory barrier.**
  - In some ways, this is the only technique you need to know, because when you use such instructions, compiler ordering is taken care of automatically.
- Just as there are many instructions which act as memory barriers, there are many different types of memory barriers to know about.
  - Indeed, not all of the above instructions produce the same kind of memory barrier – leading to another possible area of confusion when writing lock-free code.
- To begin with, consider the architecture of a typical multicore system.
  - Here’s a device with two cores, each having 32 KiB of private L1 data cache. There’s 1 MiB of L2 cache shared between both cores, and 512 MiB of main memory.

![original post image](https://preshing.com/images/cpu-diagram.png)

- A **multicore system is a bit like a group of programmers collaborating on a project using a bizarre kind of source control strategy.**
  - For example, the above dual-core system corresponds to a scenario with just two programmers, Larry & Sergey.
  - We have a shared, central repository – this represents a combination of main memory and the shared L2 cache.
  - Larry has a complete working copy of the repository on his local machine, and so does Sergey – **these (effectively) represent the L1 caches attached to each CPU core.**
  - There’s also a scratch area on each machine, to privately keep track of registers and/or local variables.
  - Our two programmers sit there, feverishly editing their working copy and scratch area, all while making decisions about what to do next based on the data they see – much like a thread of execution running on that core.

- Which brings us to the (weird) source control strategy.
  - As Larry and Sergey modify their working copies of the repository, **their modifications are constantly leaking in the background, to and from the central repository, at totally random times.**
  - Once Larry edits the file X, **his change will leak to the central repository, but there’s no guarantee about when it will happen.**
  - It might happen immediately, or it might happen much, much later.
  - He might go on to edit other files, say Y and Z, and those modifications might leak into the repository before X gets leaked.
  - In this manner, stores are effectively reordered on their way to the repository.
  - Similarly, on Sergey’s machine, there’s no guarantee about the timing or the order in which those changes leak back from the repository into his working copy. In this manner, loads are effectively reordered on their way out of the repository.
  - Now, if each programmer works on completely separate parts of the repository, neither programmer will be aware of these background leaks going on, or even of the other programmer’s existence. That would be analogous to running two independent, single-threaded processes. In this case, the cardinal rule of memory ordering is upheld.
  - The analogy becomes more useful once our programmers start working on the same parts of the repository.
- Let’s revisit the example I gave in an earlier post. X and Y are global variables, both initially 0:
  - Think of X and Y as files which exist on Larry’s working copy of the repository, Sergey’s working copy, and the central repository itself.
  - Larry writes 1 to his working copy of X and Sergey writes 1 to his working copy of Y at roughly the same time.
  - If neither modification has time to leak to the repository and back before each programmer looks up his working copy of the other file, they’ll end up with both r1 = 0 and r2 = 0.
  - This result, which may have seemed counterintuitive at first, actually becomes pretty obvious in the source control analogy.

# Types of Memory Barrier
- Fortunately, Larry and Sergey are not entirely at the mercy of these random, unpredictable leaks happening in the background. They also have the ability to issue special instructions, called **fence instructions**, which act as **memory barriers**.
- For this analogy, it’s sufficient to define **four types of memory barrier, and thus four different fence instructions.**
- Each type of memory barrier is **named after the type of memory reordering it’s designed to prevent**:
  - for example, **#StoreLoad is designed to prevent the reordering of a store followed by a load**.
- These four categories map pretty well to specific instructions on real CPUs – though not exactly.
- Most of the time, a real CPU instruction acts as some combination of the above barrier types, possibly in addition to other effects.
- In any case, once you understand these four types of memory barriers in the source control analogy, you’re in a good position to understand a large number of instructions on real CPUs, as well as several higher-level programming language constructs.

## #LoadLoad
- A **LoadLoad** barrier effectively prevents **reordering of loads performed before the barrier with loads performed after the barrier**.
  - In our analogy, the **#LoadLoad** fence instruction is basically equivalent to **a pull from the central repository**.
  - Think git pull, hg pull, p4 sync, svn update or cvs update, all acting on the entire repository.
  - If there are any merge conflicts with his local changes, let’s just say they’re resolved randomly.
  - Mind you, there’s no guarantee that #LoadLoad will pull the latest, or head, revision of the entire repository!
  - It could very well pull an older revision than the head, as long as that revision is at least as new as the newest value which leaked from the central repository into his local machine.
  - This may sound like a weak guarantee, but it’s still a perfectly good way to prevent seeing stale data.
  - Consider the classic example, where Sergey checks a shared flag to see if some data has been published by Larry. If the flag is true, he issues a #LoadLoad barrier before reading the published value:
```cpp
if (IsPublished)                   // Load and check shared flag
{
    LOADLOAD_FENCE();              // Prevent reordering of loads
    return Value;                  // Load published value
}
```
  - Obviously, this example depends on having the IsPublished flag leak into Sergey’s working copy by itself. It doesn’t matter exactly when that happens; once the leaked flag has been observed, he issues a #LoadLoad fence to prevent reading some value of Value which is older than the flag itself.



# Fences are Memory Barriers
- The key idea of a `std::atomic_thread_fence` is, to establish synchronization and ordering constraints between threads **without an atomic operation**.
- `std::atomic_thread_fence` are simply called **fences** or **memory barriers**.
- A `std::atomic_thread_fence` prevents specific operations overcoming a certain memory barrier.

## What kind of operations?
- There are four different ways to combine load and store operations:
  - Load-Load: A load followed by a load.
  - Load-Store: A load followed by a store.
  - Store-Load: A store followed by a load.
  - Store-Store: A store followed by a store.
- Of course, there are more complex operations, consisting of a load and store part (`count++`). But these operations didn't contradict my general classification.

## What kind of memory barriers?
- In case you place memory barriers between two operations like Load-Load, Load-Store, Store-Load or Store-Store, you have the **guarantee**, that specific Load-Load, Load-Store, Store-Load or Store-Store **operations can not be reordered.**
- The risk of reordering is always given if non-atomics or atomics with relaxed semantic are used.
- Three kinds of memory barriers:
  - Typically, **full fence**, **acquire fence** and **release fence**.
  - Note: **Acquire is a load, release is a store operation.**

## Types of memory barriers and their guarantee
- **Full fence**: A full fence `std::atomic_thread_fence()` a.k.a , or `std::atomic_thread_fence(std::memory_order_seq_cst)` and `std::atomic_thread_fence(std::memory_order_acq_rel)` between two arbitrary operations prevents the reordering of these operations. But that guarantee **will not hold for Store-Load** operations. They can be reordered.
- **Acquire fence**: An acquire fence `std::atomic_thread_fence(std::memory_order_acquire)` prevents, that a read operation before an acquire fence can be reordered with a reading or write operation after the acquire fence.
- **Release fence**: A release fence `std::memory_thread_fence(std::memory_order_release)` prevents, that a read or write operation before a release fence can be reordered with a write operation after a release fence.

- Acquire and release fences guarantees **similar** synchronization and ordering constraints as atomics with acquire-release semantic.
  - Only "similar", because the differences are in the details.
  - The most obvious difference between acquire and release memory barriers (fences) and atomics with acquire-release semantic is that **memory barriers need no operations on atomics.**
  - But there is a more subtle difference. The acquire and release **memory barriers are more heavyweight.**

# Atomic operations versus memory barriers
> Define: "acquire operations" when I use memory barriers or atomic operations with acquire semantic. The same will hold for release operations.
- The key idea of an acquire and a release operation is, that it establishes synchronizations and ordering constraints between thread.
- This will also hold for atomic operations with relaxed semantic or non-atomic operations.
- So you see, the acquire and release operations come in pairs. In addition, for the operations on atomic variables with acquire-release semantic must hold that these act on the same atomic variable.
- Said that I will in the first step look at these operations in isolation. I start with the acquire operation.
