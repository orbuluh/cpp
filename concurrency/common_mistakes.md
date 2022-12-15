# Common multithreading mistakes

> from [acodersjourney.com by Deb Haldar: Top 20 C++ multithreading mistakes and how to avoid them](https://www.acodersjourney.com/top-20-cplusplus-multithreading-mistakes/)

---

## Mistake # 1: Not using `join()` to wait for background threads before terminating an application

```cpp
void LaunchRocket() { cout << "Launching Rocket\n"; }
int main() {
  thread t1(LaunchRocket);
  // t1.join(); // somehow we forgot to join this to main thread - will cause a
  // crash.
  return 0;
}
```

- At the end of the main function, thread t1 goes out of scope and the thread destructor is called. Inside the destructor, a check is performed to see if thread `t1` is joinable.
- A joinable thread is a thread that has not been detached. If the thread is joinable, we call `std::terminate`.

Resolution 1: Join the thread t1 to the main thread.

```cpp
int main() {
  thread t1(LaunchRocket);
  t1.detach();  // detach t1 from main thread
  return 0;
}
```

Resolution 2: Detach the thread t1 from the main thread and let it continue as a daemon thread

```cpp
int main() {
  thread t1(LaunchRocket);
  t1.detach();  // detach t1 from main thread
  return 0;
}
```

---

## Mistake # 2: Trying to join a thread that has been previously detached

- If you have detached a thread and at some point, you cannot rejoin it to the main thread.
- This'll not cause a compilation error (which would have been nice!); instead it'll crash your program.

```cpp
void LaunchRocket() { cout << "Launching Rocket" << endl; }
int main() {
  thread t1(LaunchRocket);
  t1.detach();
  //..... 100 lines of code
  t1.join();  // CRASH !!!
  return 0;
}
```

> :brain: The solution is to **always check if a thread is join-able before trying to join it to the calling thread.**

```cpp
int main() {
  thread t1(LaunchRocket);
  t1.detach();
  //..... 100 lines of code

  if (t1.joinable()) {
    t1.join();
  }

  return 0;
}
```

---

## Mistake # 3: Not realizing that `std::thread::join()` blocks the calling thread

In real world applications, you often need to fork worker threads for long running operations handling network I/O or waiting for a button press from the user etc.

- Calling join on these worker threads in your main application (UI thread handling rendering) can **cause the application to freeze**. Often there are better ways to handle this.
- For example, in a GUI application, a worker thread that finishes can post a message to the UI thread. The UI thread itself has a message loop processing messages like mouse moves, button clicks and so on. This message loop can also receive the messages from the worker threads and can react to them without the necessity of making a blocking join call.

---

## Mistake # 4: Thinking that thread function arguments are pass by reference by default

- Thread function arguments are **by default pass by value.** So if you need the change persisted in the arguments passed in, you'll need to pass them by reference using `std::ref()`.

---

## Mistake # 5: Not protecting shared data or shared resources with a critical section (eg. `mutex`)

---

## Mistake # 6: Forgetting to release locks after a critical section

- all the other threads that are waiting on that resource will be blocked indefinitely and the program might hang.
- Programming errors happen and for this reason it is never preferable to use the lock/unlock syntax on a mutex directly. Instead, you should use `std::lock_guard` which uses RAII style to manage the duration of mutex lock.

---

## Mistake # 7: Not keeping critical sections as compact and small as possible

---

## Mistake # 8 : Not acquiring multiple locks in the same order

- This is one of the most common causes of deadlock, a situation where **threads block indefinitely because they are waiting to acquire access to resources currently locked by other blocked threads.**

|| thread A | thread B |
|---|---       | ---      |
|1| lock A   | lock B   |
|2| do sth   | do sth   |
|3| lock B   | lock A   |
|4| do sth   | do sth   |
|5| unlock B | unlock A |
|6| unlock A | unlock B |

- If execute in a way that thread A lock A, then thread B lock B, then thread A will never be able to lock B, neither do thread B will ever be able to lock A. Both of them stuck on step 3


The best thing to do is to **structure your code in such a way that all locks are acquired in the same order.** Depending on your situation,you can also employ the following strategies:

1. Acquire locks together if both need to be acquired :

```cpp
std::scoped_lock lock{mutexA, mutexB};
```

2. You can use a timed mutex where you can mandate that a lock be released after a timeout if it's not already available.

---

## Mistake # 9: Trying to acquire a std::mutex twice

**Trying to acquire a mutex twice will cause undefined behavior.**

- In most debug implementations, it'll likely result in a crash.
- For example,in the code below, `LaunchRocket()` locks a mutex and then calls `StartThruster()`. What's interesting is that there will be no issue in the normal code path – the problem will only happen when the exception codepath is triggered, in which case we'll get in an undefined state/crash.

```cpp
std::mutex mu;
static int counter = 0;

void StartThruster() {
  try {
    // Some operation to start thruster
  } catch (...) {
    std::lock_guard<std::mutex> lock(mu);
    std::cout << "Launching rocket" << std::endl;
  }
}

void LaunchRocket() {
  std::lock_guard<std::mutex> lock(mu);
  counter++;
  StartThruster();
}

int main() {
  std::thread t1(LaunchRocket);
  t1.join();
  return 0;
}
```

- The fix is to structure your code in such a way that it does not try to acquire a previously locked mutex. A superficial solution might be to just use a `std::recursive_mutex` — but this is almost always indicative of a bad design.

---

## Mistake # 10: Using mutexes when `std::atomic` types will suffice

- When you have simple data types that needs to be updated, for example, a simple bool or a integer counter, using std:atomic will almost yield better performance than using a mutex.

---

## Mistake # 11: Creating and Destroying a lot of threads directly when using a thread pool is available

- Creating and deleting threads are expensive in terms of the CPU time. Imagine trying to create a thread when the system is trying to perform a complex process like rendering graphics or calculating game physics.
- A technique often employed is to create a pool of preallocated threads that can handle routine tasks like logging to disks or sending data across network throughout the life of the process.
- The other benefit of using threadpool threads instead of spinning your own is that
  - you don't have to worry about oversubscription whereby you can affect system performance.
  - Also, all the gory details of thread lifecycle management are taken care off for you, which would mean less code and less BUGS !
- Two of the most popular libraries that implements thread pools are Intel Thread Building Blocks(TBB) and Microsoft Parallel Patterns Library(PPL).

---

## Mistake # 12: Not handling exceptions in background threads

- Exceptions thrown in one thread cannot be caught in another thread.
- Let's assume we have a function that can throw an exception. If we execute this function in a separate thread forked from main and expect to catch any exception from this thread in the main thread, it's not going to work.

```cpp
void LaunchRocket() { throw std::runtime_error("Catch me in MAIN"); }
int main() {
  try {
    std::thread t1(LaunchRocket);
    t1.join();
  } catch (const std::exception &ex) {
    std::cout << "Thread exited with exception: " << ex.what() << "\n";
  }
  return 0;
}
```

- The above program will crash and the catch block in main() will do nothing to handle the exception thrown thread t1.
- The solution is to use the C++11 feature `std::exception_ptr` to capture exception thrown in a background thread.

```cpp
static std::exception_ptr globalExceptionPtr = nullptr;
void LaunchRocket() {
  try {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    throw std::runtime_error("Catch me in MAIN");
  } catch (...) {
    // Set the global exception pointer in case of an exception
    globalExceptionPtr = std::current_exception();
  }
}
int main() {
  std::thread t1(LaunchRocket);
  t1.join();
  if (globalExceptionPtr) {
    try {
      //rethrow the exception referenced by exception_ptr parameter.
      std::rethrow_exception(globalExceptionPtr);
    } catch (const std::exception &ex) {
      std::cout << "Thread exited with exception: " << ex.what() << "\n";
    }
  }
  return 0;
}
```

- Rethrowing the referenced exception **does not have to be done in the same thread that generated the referenced exception in the first place**, which makes this feature perfectly suited for handling exceptions across different threads.
- The code below achieves safe handling of exceptions in background thread.

---

## Mistake # 13: Using threads to simulate Async jobs when `std::async` will do

- If you just need some code executed asynchronously i.e. without blocking execution of Main thread, your best bet is to use the `std::async` functionality to execute the code.
- The same could be achieved by creating a thread and passing the executable code to the thread via a function pointer or lambda parameter. However, in the later case, **you're responsible for managing creation and joining/detaching of them thread , as well as handling any exceptions that might happen in the thread.**
- If you use `std::async` , you just get rid of all these hassels and also dramatically reduce chances of getting into a deadlock scenario.

- Another huge advantage of using `std::async` is the ability to get the result of the task communicated back to the calling thread via a `std::future` object.
- For example, assuming we have a function `ConjureMagic` which returns an `int`, we can spin an async task that sets a `std::future` when it's done and we can extract the result from that `std::future` in our calling thread when at an opportune time.


```cpp
// spin an async task and get a handle to the future
std::future asyncResult2 = std::async(&ConjureMagic);
//... do some processing while the future is being set
// Get the result from the future
 int v = asyncResult2.get();
```

- On the contrary, getting the result back from a worker thread to a calling thread is much more cumbersome. The two options include:
  - Passing reference to a result variable to the thread in which the thread stores the results.
  - Store the result inside a class memeber variable of a function object which can be retrieved once the thread has finished executing.

- On the performance front, Kurt Guntheroth found that creating a thread is 14 times more expensive than using an async.
- To summarize, use `std::async` by default unless you can find good justification for using `std::thread` directly.

---

## Mistake # 14: Not using `std::launch::async` if asynchronicity is desired

- `std::async` is a bit of a misnomer because the function in it's default form may not execute in an asynchronous way !

There are two launch policies for `std::async`:

- `std::launch::async` : The task is launched immediately in a separate thread
- `std::launch::deferred`: The task is **not** launched immediately, but is **deferred until a `.get()` or `.wait()` call is made on the `future` returned by the `std::async`**. At the point such a call is made, the task is executed synchronously.

When `std::async` is launched with default parameters, it's a combination of these two policies which essentially makes the behavior unpredictable. There's a set of other complications that tags along using `std:async` with default launch parameters as well – these include,

- inability to predict whether thread local variables are properly accessed,
- the async task running the risk of not being run at all because .get() or .wait() may not get called along all codepaths and loops which wait for the future status to be ready never finishing because the future returned by `std::async` may start off in a deferred state.

- So, to avoid all these complications, ALWAYS launch `std::async` with the `std::launch::async` launch parameter.

```cpp
//run myFunction using default std::async policy (not good)
auto myFuture = std::async(myFunction);

//run myFunction asynchronously (prefer)
auto myFuture = std::async(std::launch::async, myFunction);
```

---

## Mistake # 15: Calling `.get()` on a `std::future` in a time sensitive code path

. If you consider this as a loop which renders data on screen, it can lead to a very bad user experience:

```cpp
int main() {
  std::future<int> myFuture = std::async(
    std::launch::async, []() {
        std::this_thread::sleep_for(std::chrono::seconds(10));
    return 8;
  });
  // Update Loop for rendering data
  while (true) {
    // Render some info on the screen
    std::cout << "Rendering Data" << std::endl;
    int val = myFuture.get();  // this blocks for 10 seconds
                               // Do some processing with Val
  }
  return 0;
}
```

- Note: There is an additional problem with the code above – it tries to poll a future a second time when it has with no shared state – because the state of the future was retrieved on the first iteration of the loop.

> :brain: The solution is to **check if the future is valid before calling `t.get()`**.

- This way we neither block on the completion of async job nor we try to poll an already retrieved future.

```cpp
int main() {
  std::future<int> myFuture = std::async(std::launch::async, []() {
    std::this_thread::sleep_for(std::chrono::seconds(10));
    return 8;
  });
  // Update Loop for rendering data
  while (true) {
    // Render some info on the screen
    std::cout << "Rendering Data" << std::endl;
    if (myFuture.valid()) { // THE FIX
      int val = myFuture.get();  // this blocks for 10 seconds
      // Do some processing with Val
    }
  }
  return 0;
}
```

---

## Mistake # 16: Not realizing that an exception thrown inside an `async` task is propagated when `std::future::get()` is invoked.

```cpp
int main() {
  std::future<int> myFuture = std::async(std::launch::async, []() {
    throw std::runtime_error("Catch me in MAIN");
    return 8;
  });
  if (myFuture.valid()) {
    int result = myFuture.get();
  }
  return 0;
}
```

- this causes a crash. The exception from async tasks is only propagated when we call get on the future. If get is not called, the exception is ignored and discarded when the future goes out of scope.

> :brain:  if your async tasks can throw, you should always wrap the call to `std::future::get()` in a try/catch block.

```cpp
int main() {
  std::future<int> myFuture = std::async(std::launch::async, []() {
    throw std::runtime_error("Catch me in MAIN");
    return 8;
  });
  if (myFuture.valid()) {
    try {
      int result = myFuture.get();
    } catch (const std::runtime_error& e) {
      std::cout << "Async task threw exception: " << e.what() << '\n';
    }
  }
  return 0;
}
```

---

## Mistake # 17: Using `std::async` when you need granular control over thread execution

- There are situations where you'd want more granular control over the thread executing your code. For example, **if you want to pin the thread to a specific CPU core** in a multi processor system
- The following piece of code sets the processor affinity of the thread to core 5 of my system.

```cpp
void LaunchRocket() { cout << "Launching Rocket" << endl; }
int main() {
  thread t1(LaunchRocket);
  DWORD result = ::SetThreadIdealProcessor(t1.native_handle(), 5);
  t1.join();
  return 0;
}
```

- This is made possible by using the `native_handle` of the `std::thread`, and passing it to an Win32 thread API function.
  - There's a bunch of other functionality exposed via the Win32 Threads API that is not exposed in `std::thread` or `std::async`.
  - `std::async` makes these underlying platform features inaccessible which makes it not suitable for more sophisticated work.

- The other option is to create a `std::packaged_task` and move it to the desired thread of execution after setting thread properties.

---

## Mistake # 18: Creating many more "Runnable" threads than available cores

- Threads can be classified into two types from design perspective – **Runnable** threads and **Waitable** threads.

- **Runnable** threads consume 100% of the CPU time of the core on which they run. **When more than one runnable thread is scheduled on a single core, they effectively time slice the CPU time of the core.**
- There is no performance gain achieved when more than one runnable thread is scheduled on a single core - in fact **there is a performance degradation due to additional context switches involved.**

- **Waitable** threads consumes only a few cycles of the core they run on while waiting for events or network I/O etc. This leaves majority of the available compute time of the CPU core unused. **That's why it's beneficial to schedule multiple waitable threads on a single core because one waitable thread can process data while others are waiting for some event to happen.** - Scheduling multiple waitable threads on a single core can provide much larger throughput from your program.

So, how do you get the number of runnable threads the system can support ? Use `std::thread::hardware_concurrency()` . This function will generally return the number of processor cores – but if will factor in cores that behave as two or more logical cores due to **hyperthreading**.

- You should use this value from your target platform to plan the max number of Runnable threads your program should concurrently use. You can also designate a core for all your waitable threads and use the remaining number of cores for runnable threads.
- For example, on a quad-core system, use one core for ALL waitable threads and use three runnable threads for the remaining three cores.
  - Depending on your thread schedulers efficiency, a few of your runnable threads might get context switched out (due to page faults etc.) leaving the core idle for some amount of time.
  - If you observe this situation during profiling, you should create a few more runnable threads than the number of your cores and tune it for your system.

---

## Mistake # 19: Using "volatile" keyword for synchronization

- The "volatile" keyword in front of a variable type declaration does not make the operations on that variable atomic or thread safe in any way. What you probably want is an `std::atomic`.

---

## Mistake # 20: Using a Lock Free architecture unless absolutely needed

- There is something about complexity that appeals to every engineer. Lock free programming sounds very sexy when compared to regular synchronization mechanisms such as mutex, condition variables, async etc. However, every seasoned C++ developer I've spoken to has had the opinion that **using lock free programming as first resort is a form of premature optimization that can come back to haunt you at the most in opportune time ( Think a crash out in production when you don't have the full heap dump !).**

- In my C++ career, there has been only one piece of tech which needed the performance of lock free code because we're on a resource constrained system where each transaction from our component needed to take no more than 10 micro seconds.

- So, before you start thinking going the lock free route, please ask yourself these three questions in order:

  - Have you considered designing your system such that it does not need a synchronization mechanism ? The best synchronization is often "No synchronization" !
  - If you do need synchronization, have you profiled your code to understand the performance characteristics ? If yes, have you tried to optimize the hot code paths?
  - Can you scale out instead of scaling up ?

- In a nutshell, for regular application development, **please consider lock free programming only when you've exhausted all other alternatives**. Another way to look at it is that if you're still making some of the the above 19 mistakes, you should probably stay away from lock free programming.