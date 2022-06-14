# CP: Concurrency and parallelism

- Here, we articulate principles and rules for using the ISO standard C++ facilities for expressing basic concurrency and parallelism.
- Threads are the machine-level foundation for concurrent and parallel programming.
  - Threads allow running multiple sections of a program independently, while sharing the same memory.
  - Concurrent programming is tricky, because protecting shared data between threads is easier said than done.
  - Making existing single-threaded code execute concurrently can be as trivial as adding `std::async` or `std::thread` strategically, or it can necessitate a full rewrite, depending on whether the original code was written in a thread-friendly way.

- The concurrency/parallelism rules in this document are designed with three goals in mind:
  - To help in writing code that is amenable to being used in a threaded environment
  - To show clean, safe ways to use the threading primitives offered by the standard library
  - To offer guidance on what to do when concurrency and parallelism aren't giving the performance gains needed

- It is also important to note that concurrency in C++ is an unfinished story.
  - C++11 introduced many core concurrency primitives,
  - C++14 and C++17 improved on them,
  - and there is much interest in making the writing of concurrent programs in C++ even easier.
  - We expect some of the library-related guidance here to change significantly over time.

## CP.1: Assume that your code will run as part of a multi-threaded program
- It's hard to be certain that concurrency isn't used now or won't be used sometime in the future.
- Code gets reused. Libraries not using threads might be used from some other part of a program that does use threads.
- Note that this rule applies most urgently to library code and least urgently to stand-alone applications. However, over time, code fragments can turn up in unexpected places.

```cpp
//Example, bad
double cached_computation(int x)
{
    // bad: these statics cause data races in multi-threaded usage
    static int cached_x = 0.0;
    static double cached_result = COMPUTATION_OF_ZERO;

    if (cached_x != x) {
        cached_x = x;
        cached_result = computation(x);
    }
    return cached_result;
}
```
- Although cached_computation works perfectly in a single-threaded environment, in a multi-threaded environment the two static variables result in data races and thus undefined behavior.

```cpp
struct ComputationCache {
  int cached_x = 0;
  double cached_result = COMPUTATION_OF_ZERO;

  double compute(int x) {
      if (cached_x != x) {
          cached_x = x;
          cached_result = computation(x);
      }
      return cached_result;
  }
};
```
- Here the cache is stored as member data of a `ComputationCache` object, rather than as shared `static` state. This refactoring essentially delegates the concern upward to the caller: a single-threaded program might still choose to have one global ComputationCache, while a multi-threaded program might have one ComputationCache instance per thread, or one per "context" for any definition of "context."
- The refactored function no longer attempts to manage the allocation of cached_x. In that sense, this is an application of the Single Responsibility Principle.
- In this specific example, refactoring for thread-safety also improved reusability in single-threaded programs. It's not hard to imagine that a single-threaded program might want two ComputationCache instances for use in different parts of the program, without having them overwrite each other's cached data.

- There are several other ways one might add thread-safety to code written for a standard multi-threaded environment (that is, one where the only form of concurrency is `std::thread`):
  - Mark the state variables as `thread_local` instead of `static`.
  - Implement concurrency control, for example, protecting access to the two static variables with a `static std::mutex`.
  - Refuse to build and/or run in a multi-threaded environment.
  - Provide two implementations: one for single-threaded environments and another for multi-threaded environments.
- Exception: Code that is never run in a multi-threaded environment.
  - Be careful: there are many examples where code that was "known" to never run in a multi-threaded program was run as part of a multi-threaded program, often years later.
  - Typically, such programs lead to a painful effort to remove data races. Therefore, code that is never intended to run in a multi-threaded environment should be clearly labeled as such and ideally come with compile or run-time enforcement mechanisms to catch those usage bugs early.

## CP.2: Avoid data races
- Unless you do, nothing is guaranteed to work and subtle errors will persist.
- In a nutshell, **if two threads can access the same object concurrently (without synchronization), and at least one is a writer (performing a non-const operation), you have a data race.**
- There are many examples of data races that exist, some of which are running in production software at this very moment. One very simple example:
```cpp
int get_id()
{
  static int id = 1;
  return id++;
}
```
- The increment here is an example of a data race. This can go wrong in many ways, including:
  - Thread A loads the value of id, the OS context switches A out for some period, during which other threads create hundreds of IDs.
  - Thread A is then allowed to run again, and id is written back to that location as A's read of id plus one.
  - Thread A and B load id and increment it simultaneously. They both get the same ID.
- **Local static variables are a common source of data races.**

```cpp
void f(fstream& fs, regex pattern) {
    array<double, max> buf;
    int sz = read_vec(fs, buf, max); // read from fs into buf
    gsl::span<double> s{buf};
    // ...
    auto h1 =
        async([&] { sort(std::execution::par, s); }); // spawn a task to sort
    // ...
    auto h2 = async([&] {
        return find_all(buf, sz, pattern);
    }); // spawn a task to find matches
    // ...
}
```
- Here, we have a (nasty) data race on the elements of `buf` (sort will both read and write).
- All data races are nasty. Here, we managed to get a data race on data on the stack. Not all data races are as easy to spot as this one.

```cpp
// code not controlled by a lock

unsigned val;

if (val < 5) {
    // ... other thread can change val here ...
    switch (val) {
    case 0: // ...
    case 1: // ...
    case 2: // ...
    case 3: // ...
    case 4: // ...
    }
}
```
- Now, a compiler that does not know that val can change will most likely implement that switch using a jump table with five entries. Then, a val outside the [0..4] range will cause a jump to an address that could be anywhere in the program, and execution would proceed there.
- **Really, "all bets are off" if you get a data race.**
- Actually, it can be worse still: by looking at the generated code you might be able to determine where the stray jump will go for a given value; **this can be a security risk.**
- There are other ways you can mitigate the chance of data races:
  - Avoid global data
  - Avoid static variables
  - More use of concrete types on the stack (and don't pass pointers around too much)
  - More use of immutable data (literals, `constexpr`, and `const`)

## CP.3: Minimize explicit sharing of writable data
- If you don't share writable data, you can't have a data race.
- The less sharing you do,
  - the less chance you have to forget to synchronize access (and get data races)
  - the less chance you have to wait on a lock (so performance can improve).
- Immutable data can be safely and efficiently shared. No locking is needed: You can't have a data race on a constant.

