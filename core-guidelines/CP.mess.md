# CP.mess: Message passing
- The standard-library facilities are quite low-level, focused on the needs of close-to the hardware critical programming using threads, mutexes, atomic types, etc.
- Most people shouldn't work at this level: it's error-prone and development is slow.
- If possible, use a higher level facility: messaging libraries, parallel algorithms, and vectorization.
- This section looks at passing messages so that a programmer doesn't have to do explicit synchronization.

- [CP.mess: Message passing](#cpmess-message-passing)
  - [CP.60: Use a `future` to return a value from a concurrent task](#cp60-use-a-future-to-return-a-value-from-a-concurrent-task)
  - [CP.61: Use `async()` to spawn concurrent tasks](#cp61-use-async-to-spawn-concurrent-tasks)

## CP.60: Use a `future` to return a value from a concurrent task
- A `future` preserves the usual function call return semantics for asynchronous tasks.
- There is no explicit locking and both correct (value) return and error (exception) return are handled simply.


## CP.61: Use `async()` to spawn concurrent tasks
- Similar to [R.12](R.md#r12-immediately-give-the-result-of-an-explicit-resource-allocation-to-a-manager-object), which tells you to avoid raw owning pointers, you **should also avoid raw threads and raw promises** where possible.
- Use a factory function such as `std::async`, which handles spawning or reusing a thread **without exposing raw threads to your own code.**

```cpp
// Example
int read_value(const std::string& filename)
{
    std::ifstream in(filename);
    in.exceptions(std::ifstream::failbit);
    int value;
    in >> value;
    return value;
}

void async_example()
{
    try {
        std::future<int> f1 = std::async(read_value, "v1.txt");
        std::future<int> f2 = std::async(read_value, "v2.txt");
        std::cout << f1.get() + f2.get() << '\n';
    } catch (const std::ios_base::failure& fail) {
        // handle exception here
    }
}
```
- Note: Unfortunately, `std::async` is not perfect.
  - For example, it doesn't use a thread pool, which means that it might fail due to resource exhaustion, rather than queuing up your tasks to be executed later.
  - However, even if you cannot use `std::async`, you should prefer to write your own future-returning factory function, rather than using raw promises.
- This example shows two different ways to succeed at using `std::future`, but to fail at avoiding raw `std::thread` management.
```cpp
// Example (bad)
void async_example() {
    std::promise<int> p1;
    std::future<int> f1 = p1.get_future();
    std::thread t1(
        [p1 = std::move(p1)]() mutable { p1.set_value(read_value("v1.txt")); });
    t1.detach(); // evil

    std::packaged_task<int()> pt2(read_value, "v2.txt");
    std::future<int> f2 = pt2.get_future();
    std::thread(std::move(pt2)).detach();

    std::cout << f1.get() + f2.get() << '\n';
}
```
- This example shows one way you could follow the general pattern set by `std::async`, in a context where `std::async` itself was unacceptable for use in production.
```cpp
// Example (good)
void async_example(WorkQueue& wq) {
    std::future<int> f1 = wq.enqueue([]() { return read_value("v1.txt"); });
    std::future<int> f2 = wq.enqueue([]() { return read_value("v2.txt"); });
    std::cout << f1.get() + f2.get() << '\n';
}
```
- Any threads spawned to execute the code of `read_value` are hidden behind the call to `WorkQueue::enqueue`.
- The **user code deals only with `future` objects, never with raw `thread`, `promise`, or `packaged_task` objects.**
