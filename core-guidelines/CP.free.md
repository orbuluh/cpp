# CP.free: Lock-free programming
- Synchronization using `mutex`es and `condition_variable`s can be relatively expensive.
- Furthermore, it can lead to deadlock. For performance and to eliminate the possibility of deadlock, we sometimes have to use the tricky low-level "lock-free" facilities that rely on briefly gaining exclusive ("`atomic`") access to memory.
- Lock-free programming is also used to implement higher-level concurrency mechanisms, such as `thread`s and `mutex`es.

- [CP.free: Lock-free programming](#cpfree-lock-free-programming)
  - [CP.100: Don't use lock-free programming unless you absolutely have to](#cp100-dont-use-lock-free-programming-unless-you-absolutely-have-to)
  - [CP.101: Distrust your hardware/compiler combination](#cp101-distrust-your-hardwarecompiler-combination)
  - [CP.102: Carefully study the literature](#cp102-carefully-study-the-literature)
  - [CP.110: Do not write your own double-checked locking for initialization](#cp110-do-not-write-your-own-double-checked-locking-for-initialization)
  - [CP.111: Use a conventional pattern if you really need double-checked locking](#cp111-use-a-conventional-pattern-if-you-really-need-double-checked-locking)

## CP.100: Don't use lock-free programming unless you absolutely have to
- It's error-prone and requires expert level knowledge of language features, machine architecture, and data structures.
```cpp
//Example, bad
extern atomic<Link*> head; // the shared head of a linked list

Link* nh = new Link(data, nullptr); // make a link ready for insertion
Link* h = head.load();              // read the shared head of the list

do {
    if (h->data <= data)
        break;    // if so, insert elsewhere
    nh->next = h; // next element is the previous head
} while (!head.compare_exchange_weak(h, nh)); // write nh to head or to h
```
- Spot the bug. It would be really hard to find through testing. Read up on the ABA problem.

- Exception: Atomic variables can be used simply and safely, as long as you are using the **sequentially consistent memory model** (`memory_order_seq_cst`), which is the default.
- Note: Higher-level concurrency mechanisms, such as `thread`s and `mutex`es are implemented using lock-free programming.
- Alternative: Use lock-free data structures implemented by others as part of some library.

## CP.101: Distrust your hardware/compiler combination
- The low-level hardware interfaces used by lock-free programming are among the hardest to implement well and among the areas where the most subtle portability problems occur.
- If you are doing lock-free programming for performance, you need to check for regressions.
- Note: Instruction reordering (static and dynamic) makes it hard for us to think effectively at this level (especially if you use relaxed memory models). 
- Experience, (semi)formal models and model checking can be useful.
- Testing - often to an extreme extent - is essential. "Don't fly too close to the sun."

## CP.102: Carefully study the literature
- With the exception of atomics and a few other standard patterns, lock-free programming is really an expert-only topic.
- Become an expert before shipping lock-free code for others to use.

- References
  - Anthony Williams: C++ concurrency in action. Manning Publications.
  - Boehm, Adve, You Don't Know Jack About Shared Variables or Memory Models , Communications of the ACM, Feb 2012.
  - Boehm, "Threads Basics", HPL TR 2009-259.
  - Adve, Boehm, "Memory Models: A Case for Rethinking Parallel Languages and Hardware", Communications of the ACM, August 2010.
  - Boehm, Adve, "Foundations of the C++ Concurrency Memory Model", PLDI 08.
  - Mark Batty, Scott Owens, Susmit Sarkar, Peter Sewell, and Tjark Weber, "Mathematizing C++ Concurrency", POPL 2011.
  - Damian Dechev, Peter Pirkelbauer, and Bjarne Stroustrup: Understanding and Effectively Preventing the ABA Problem in Descriptor-based Lock-free Designs. 13th IEEE Computer Society ISORC 2010 Symposium. May 2010.
  - Damian Dechev and Bjarne Stroustrup: Scalable Non-blocking Concurrent Objects for Mission Critical Code. ACM OOPSLA'09. October 2009
  - Damian Dechev, Peter Pirkelbauer, Nicolas Rouquette, and Bjarne Stroustrup: Semantically Enhanced Containers for Concurrent Real-Time Systems. Proc. 16th Annual IEEE International Conference and Workshop on the Engineering of Computer Based Systems (IEEE ECBS). April 2009.
  - Maurice Herlihy, Nir Shavit, Victor Luchangco, Michael Spear, "The Art of Multiprocessor Programming", 2nd ed. September 2020

## CP.110: Do not write your own double-checked locking for initialization
- Since C++11, **static local variables are now initialized in a thread-safe way**.
- When combined with the RAII pattern, static local variables can replace the need for writing your own double-checked locking for initialization.
- `std::call_once` can also achieve the same purpose.
- **Use either static local variables of C++11 or `std::call_once` instead of writing your own double-checked locking for initialization.**

```cpp
void f() {
    static std::once_flag my_once_flag;
    std::call_once(my_once_flag, []() {
        // do this only once
    });
    // ...
}

void f() {
    // Assuming the compiler is compliant with C++11
    static My_class my_object; // Constructor called only once
    // ...
}

class My_class {
  public:
    My_class() {
        // do this only once
    }
};
```

## CP.111: Use a conventional pattern if you really need double-checked locking
- Double-checked locking is easy to mess up. If you really need to write your own double-checked locking, then do it in a conventional pattern.
- The uses of the double-checked locking pattern that are not in violation of [CP.110](#cp110-do-not-write-your-own-double-checked-locking-for-initialization)  arise **when a non-thread-safe action is both hard and rare, and there exists a fast thread-safe test that can be used to guarantee that the action is not needed, but cannot be used to guarantee the converse.**

```cpp
// Example, bad
// The use of volatile does not make the first check thread - safe,
// see also CP .200 : Use volatile only to talk to non-C++ memory

std::mutex action_mutex;
volatile bool action_needed;

if (action_needed) {
    std::lock_guard<std::mutex> lock(action_mutex);
    if (action_needed) {
        take_action();
        action_needed = false;
    }
}

// Example, good
mutex action_mutex;
atomic<bool> action_needed;

if (action_needed) {
    std::lock_guard<std::mutex> lock(action_mutex);
    if (action_needed) {
        take_action();
        action_needed = false;
    }
}
```
- Fine-tuned memory order might be beneficial where acquire load is more efficient than sequentially-consistent load
```cpp
mutex action_mutex;
atomic<bool> action_needed;

if (action_needed.load(memory_order_acquire)) {
    lock_guard<std::mutex> lock(action_mutex);
    if (action_needed.load(memory_order_relaxed)) {
        take_action();
        action_needed.store(false, memory_order_release);
    }
}
```
