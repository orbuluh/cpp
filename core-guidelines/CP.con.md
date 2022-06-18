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


## CP.24: Think of a thread as a global container (when it's detached)
- To maintain pointer safety and avoid leaks, we need to consider what pointers are used by a thread.
- If a thread is detached, we can safely pass pointers to static and free store objects (only).
  - Note: Even objects with static storage duration can be problematic if used from detached threads:
    - if the thread continues until the end of the program, it might be running concurrently with the destruction of objects with static storage duration, and thus accesses to such objects might race.
- Note: This rule is redundant if you don't `detach()` and use `gsl::joining_thread`.
- If we cannot prove that a thread does not `detach()`, we must assume that it does and that it outlives the scope in which it was constructed; After that, the usual lifetime and ownership (for global objects) enforcement applies.


## CP.25: Prefer `gsl::joining_thread` over `std::thread`
- A `joining_thread` is a thread that joins at the end of its scope.
- Detached threads are hard to monitor.
- It is harder to ensure absence of errors in detached threads (and potentially detached threads).
- Note: Make "immortal threads" globals, put them in an enclosing scope, or put them on the free store rather than `detach()`. **Don't detach.**
- Note: Because of old code and third party libraries using `std::thread`, this rule can be hard to introduce.


## CP.26: Don't `detach()` a thread
- Often, the need to outlive the scope of its creation is inherent in the threads task, but implementing that idea by detach makes it harder to monitor and communicate with the detached thread.
- In particular, it is harder (though not impossible) to ensure that the thread completed as expected or lives for as long as expected.

```cpp
void heartbeat();

void use()
{
    std::thread t(heartbeat);             // don't join; heartbeat is meant to run forever
    t.detach();
    // ...
}
```
- This is a reasonable use of a thread, for which `detach()` is commonly used.
- There are problems, though. How do we monitor the detached thread to see if it is alive? Something might go wrong with the heartbeat, and losing a heartbeat can be very serious in a system for which it is needed.
- So, we need to communicate with the heartbeat thread (e.g., through a stream of messages or notification events using a `condition_variable`).
- An alternative, and usually superior solution is to control its lifetime by placing it in a scope outside its point of creation (or activation).

```cpp
void heartbeat();

gsl::joining_thread t(heartbeat);             // heartbeat is meant to run "forever"
```
- This heartbeat will (barring error, hardware problems, etc.) run for as long as the program does.
- Sometimes, we need to separate the point of creation from the point of ownership:

```cpp
void heartbeat();
unique_ptr<gsl::joining_thread> tick_tock {nullptr};

void use()
{
    // heartbeat is meant to run as long as tick_tock lives
    tick_tock = make_unique<gsl::joining_thread>(heartbeat);
    // ...
}
```


## CP.31: Pass small amounts of data between threads by value, rather than by reference or pointer
- A small amount of data is cheaper to copy and access than to share it using some locking mechanism.
- Copying naturally gives unique ownership (simplifies code) and eliminates the possibility of data races.

```cpp
Example
string modify1(string);
void modify2(string&);

void fct(string& s)
{
    auto res = async(modify1, s);
    async(modify2, s);
}
```

- The call of `modify1` involves copying two string values; the call of `modify2` does not.
- On the other hand, the implementation of `modify1` is exactly as we would have written it for single-threaded code, whereas the implementation of `modify2` will need some form of locking to avoid data races.
- If the string is short (say 10 characters), the call of `modify1` can be surprisingly fast; essentially all the cost is in the thread switch.
- If the string is long (say 1,000,000 characters), copying it twice is probably not a good idea.
- Note that this argument has nothing to do with async as such. It applies equally to considerations about whether to use message passing or shared memory.


## CP.32: To share ownership between unrelated threads use `shared_ptr`
- If threads are unrelated (that is, not known to be in the same scope or one within the lifetime of the other) **and they need to share free store memory that needs to be deleted**, a `shared_ptr` (or equivalent) is the only safe way to ensure proper deletion.
- Note: A static object (e.g. a global) can be shared because it is not owned in the sense that some thread is responsible for its deletion.
- An object on free store that is never to be deleted can be shared.
- An object owned by one thread can be safely shared with another as long as that second thread doesn't outlive the owner.

## CP.40: Minimize context switching
- Context switches are expensive.

## CP.41: Minimize thread creation and destruction
- Thread creation is expensive.

```cpp
void worker(Message m)
{
    // process
}

void dispatcher(istream& is)
{
    for (Message m; is >> m; )
        run_list.push_back(new thread(worker, m));
}
```
- This spawns a thread per message, and the run_list is presumably managed to destroy those tasks once they are finished.s
- Instead, we could have a set of pre-created worker threads processing the messages

```cpp
Sync_queue<Message> work;

void dispatcher(istream& is)
{
    for (Message m; is >> m; )
        work.put(m);
}

void worker()
{
    for (Message m; m = work.get(); ) {
        // process
    }

}

void workers()  // set up worker threads (specifically 4 worker threads)
{
    joining_thread w1 {worker};
    joining_thread w2 {worker};
    joining_thread w3 {worker};
    joining_thread w4 {worker};
}
```
- If your system has a good thread pool, use it.
- If your system has a good message queue, use it.


## CP.42: Don't wait without a condition
- A wait without a condition can miss a wakeup or wake up simply to find that there is no work to do.

```cpp
std::condition_variable cv;
std::mutex mx;

void thread1()
{
    while (true) {
        // do some work ...
        std::unique_lock<std::mutex> lock(mx);
        cv.notify_one();    // wake other thread
    }
}

void thread2()
{
    while (true) {
        std::unique_lock<std::mutex> lock(mx);
        cv.wait(lock);    // might block forever
        // do work ...
    }
}
```
- Here, if some other thread consumes `thread1`'s notification, `thread2` can wait forever.

```cpp
template<typename T>
class Sync_queue {
public:
    void put(const T& val);
    void put(T&& val);
    void get(T& val);
private:
    mutex mtx;
    condition_variable cond;    // this controls access
    list<T> q;
};

template<typename T>
void Sync_queue<T>::put(const T& val)
{
    lock_guard<mutex> lck(mtx);
    q.push_back(val);
    cond.notify_one();
}

template<typename T>
void Sync_queue<T>::get(T& val)
{
    unique_lock<mutex> lck(mtx);
    cond.wait(lck, [this] { return !q.empty(); });    // prevent spurious wakeup
    val = q.front();
    q.pop_front();
}
```
- Now if the queue is empty when a thread executing `get()` wakes up (e.g., because another thread has gotten to `get()` before it), it will immediately go back to sleep, waiting.

