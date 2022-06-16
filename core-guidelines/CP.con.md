# CP.con: Concurrency
This section focuses on relatively ad-hoc uses of multiple threads communicating through shared data.
- For parallel algorithms, see [parallelism](CP.par.md)
- For inter-task communication without explicit sharing, see [messaging](CP.mess.md)
- For vector parallel code, see [vectorization](CP.vec.md)
- For lock-free programming, see [lock free](CP.free.md)


## CP.20: Use RAII, never plain `lock()`/`unlock()`
- Avoids nasty errors from unreleased locks.

## CP.21: Use `std::lock()` or `std::scoped_lock` to acquire multiple mutexes
- To avoid deadlocks on multiple mutexes.
```cpp
// This is asking for deadlock:
// thread 1
lock_guard<mutex> lck1(m1);
lock_guard<mutex> lck2(m2);

// thread 2
lock_guard<mutex> lck2(m2);
lock_guard<mutex> lck1(m1);
```
- Instead, use `lock()`:
```cpp
// thread 1
lock(m1, m2);
lock_guard<mutex> lck1(m1, adopt_lock);
lock_guard<mutex> lck2(m2, adopt_lock);


// thread 2
lock(m2, m1);
lock_guard<mutex> lck2(m2, adopt_lock);
lock_guard<mutex> lck1(m1, adopt_lock);
```
- or (better, but C++17 only):

```cpp
// thread 1
scoped_lock<mutex, mutex> lck1(m1, m2);

// thread 2
scoped_lock<mutex, mutex> lck2(m2, m1);
```
- Here, the writers of `thread1` and `thread2` are still not agreeing on the order of the mutexes, but **order no longer matters**.
- In real code, mutexes are **rarely** named to conveniently remind the programmer of an i**ntended relation and intended order of acquisition**.
- In real code, mutexes are **not always conveniently acquired on consecutive lines.**
- Note In C++17 it's possible to let `lock_guard` do the template parameter deduction
```cpp
lock_guard lck1(m1, adopt_lock);
```

## CP.22: Never call unknown code while holding a lock (e.g., a callback)
- Reason - If you don't know what a piece of code does, you are risking `deadlock`.

```cpp
void do_this(Foo* p)
{
    lock_guard<mutex> lck {my_mutex};
    // ... do something ...
    p->act(my_data);
    // ...
}
```
- If you don't know what `Foo::act` does (maybe it is a virtual function invoking a derived class member of a class not yet written), it might call `do_this` (recursively) and cause a deadlock on `my_mutex`.
- Maybe it will lock on a different mutex and not return in a reasonable time, causing delays to any code calling `do_this`.
- Example: A common example of the "calling unknown code" problem is a call to a function that tries to gain locked access to the same object. Such problem can often be solved by using a `recursive_mutex`.
- For example:
```cpp
recursive_mutex my_mutex;

template<typename Action>
void do_something(Action f)
{
    unique_lock<recursive_mutex> lck {my_mutex};
    // ... do something ...
    f(this);    // f will do something to *this
    // ...
}
```
- If, as it is likely, f() invokes operations on `*this`, we must make sure that the object's invariant holds before the call.

## CP.23: Think of a joining thread as a scoped container
- To maintain pointer safety and avoid leaks, we need to consider what pointers are used by a thread.
- If a thread joins, we can safely pass pointers to objects in the scope of the thread and its enclosing scopes.
- By "OK" we mean that the object will **be in scope ("live")** for as long as a thread can use the pointer to it.
- The fact that threads run concurrently doesn't affect the lifetime or ownership issues here; these threads can be seen as just a function object called from some_fct.
- A `gsl::joining_thread` is a `std::thread` with a destructor that joins and that cannot be `detached()`.
```cpp
//Example
void f(int* p) {
    // ...
    *p = 99;
    // ...
}
int glob = 33;

void some_fct(int* p) {
    int x = 77;
    gsl::joining_thread t0(f, &x);    // OK
    gsl::joining_thread t1(f, p);     // OK
    gsl::joining_thread t2(f, &glob); // OK
    auto q = make_unique<int>(99);
    gsl::joining_thread t3(f, q.get()); // OK
    // ...
}
```


