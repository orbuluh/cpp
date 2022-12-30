# Concurrency in C++: A Programmer’s Overview (part 2 of 2) - Fedor Pikus

## What is an atomic operation?

- Atomic operation is an operation that is guaranteed to **execute as a single transaction. E.g. Other threads will see the state of the system before the operation started or after it finished, but cannot see any intermediate state.**
- At the low level, atomic operations are special hardware instructions (hardware guarantees atomicity)
- NOTE: This is a general concept, not limited to hardware instructions (example: database transactions)

```txt

    int x = 0;
-----------------------
thread 1  |   thread 2
   ++x    |      ++x
-----------------------
    x = ?
```

- Increment is a "read-modify-write" operations: read x from memory, add 1 to x, write new x to memory.
- Normally Read-modify-write increment is non-atomic, so there could be a data race (which will resolve to undefined behavior!)

```txt

    int x = 0;
------------------------------------------
thread 1            |   thread 2
int tmp = x; //0    |   int tmp = x; //0
++tmp; // 1         |   ++tmp; // 1
x = tmp; // 1       |   x = tmp; // 1
------------------------------------------
    x = 1 (!!!)
```

## More insidious atomic operation example

- On x86, in practice, for built-in types (int, long, <= 64bytes), reads and writes do not have to be atomic (though theoretically it's UB)
- E.g. when you write something into an int, and read it from other thread. The reader thread should also not see the half-result of the write. (But if you are doing, say, int 128 bytes, you don't have this guarantee)

## What's really going on for atomic

Say we have 3 threads working on 3 cores, and all accessing x. The mental model should be like:

```txt
CPU core 1  | CPU core 2  | CPU core 3  |
L1 cache    | L1 cache    | L1 cache    |
L2 cache    | L2 cache    | L2 cache    |
-----------------------------------------
L3 cache
-----------------------------------------
Main memory
-----------------------------------------
```

- Variable x is in the main memory, when we are going to do ++x, x is being copied into L3 cache -> L2 cache -> L1 cache, then being ++'ed by the CPU.
- All the L2 cache/L1 cache of each core/thread will all have the copy.
- What atomic do is that, if core 1 is ++'ing x, it will make sure the value is flush back to main memory, and for thread 2/core 2, thread 3/core 3, they will have to read the x again all the way from L3 cache -> L2 cache -> L1 cache

## What C++ types can be made atomic?

- Any trivially copyable and copyable/movable type can be made atomic (Anything else will not compile)

**What is trivially copyable?**

- Continuous chunk of memory
- Copying the object means copying all bits (`memcpy`)
- No virtual functions, `noexcept` constructor

So:

```cpp
std::atomic<int> i; // OK
std::atomic<double> x; // OK
struct S { long x; long y; };
std::atomic<S> s; // ALSO OK
```

## What operations can be done on these types and are all operations on atomic types atomic?

There are actually 4 group of operations

- Assignment (read and write) - always supported regardless of `T`
- Special atomic operations which are exclusively to atomic
- Operations depend on the type `T`
- Operation that just won't be supported by atomic

Think about below ...

- One can't even be compiled
- Two of these are not the same as the others

```cpp
std::atomic<int> x{0};

++x;
x++;
x += 1;
x |= 2;
x *= 2;
int y = x * 2;
x = y + 1;
x = x + 1;
x = x * 2;
```

- All operations on the atomic variable `x` are atomic below (unless it's the case that it's not supported)

```cpp
std::atomic<int> x{0};

++x;     // Atomic pre-increment
x++;     // Atomic post-increment
x += 1;  // Atomic increment
x |= 2;  // Atomic bit set
x *= 2;  // <--- no atomic multiplication at all (won't compile)
int y = x * 2;  // atomic read of x
x = y + 1;  // atomic write of x
x = x + 1;  // atomic read followed by atomic write, not single atomic
x = x * 2;  // atomic read followed by atomic write, not single atomic
```

## `std::atomic<T>` and overloaded operators

- `std::atomic<T>` provides operator overloads only for atomic operations (incorrect code like `*=-` does not compile as expect)
- Unfortunately, any expression with atomic variables will not be computed atomically (easy to make mistakes - like `x = x + 1` and `x = x * 2;` --> they are not atomic like `x += 1` and `x *= 2` doesn't even compile. But they can be compiled)

> :brain::rotating_light: Don't use overloaded operators on atomic!

- `++x;` is the same as `x += 1;` is the same as `x = x + 1;` - unless x is atomic

## What operations can be done on `std::atomic<T>` for other types?

- Assignment and copy (read and write) for all types (Built-in and user-defined)
- Increment and decrement for raw pointers
- Addition, subtraction, and bitwise logic operations for integers
  - (`++`, `+=`, `-`, `-=`, `|=`, `&=`, `^=`)
- `std::atomic<bool>` is valid atomic on single byte, no special operations
- `std::atomic<double>` is valid, but it has no special operations
  - No atomic increment for floating-point numbers until C++20
  - (Since C++20) Atomic floating-point math is supported, but with caveat that result could differ from non-atomic
- Atomic wait and notify (C++20)

## What "other operations" can be done on `std::atomic<T>`?

- Use explicit read and writes:

```cpp
std::atomic<T> x;
T y = x.load(); // Same as T y = x;
x.store(y);  // same as x = y;
```

- Atomic exchange

```cpp
T z = x.exchange(y); // Atomically do z = x; x = y;
```

- Conditional exchange: compare-and-swap (The key to most lock-free algorithms)

```cpp
bool success = x.compare_exchange_strong(y, z); // T& y
// If x == y, make x = z and return true
// otherwise set y = x and return false
```

=============TEMPORARY @ around 15:38=============================






How fast are atomic operations?

Are atomic operations slower than non-atomic?

Are atomic operations faster than locks?

Is "atomic" same as "lock-free"?

If atomic operations avoid locks, there is no waiting, right?













