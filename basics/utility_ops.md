[Code example](utility_ops.h)



## `std::exchange`

> Taking notes from [std::exchange Patterns: Fast, Safe, Expressive, and Probably Underused by Ben Deane](https://www.fluentcpp.com/2020/09/25/stdexchange-patterns-fast-safe-expressive-and-probably-underused/)

> :brain: Whenever you find yourself writing `std::swap`, consider: do you really need that temporary? If not, use `std::exchange` instead.

- When we write `i++`, it’s exactly the same as writing `std::exchange(i, i+1)`
- Although postfix increment may not be nearly as widespread in a typical codebase as prefix increment, we usually have no problems using it or reasoning about its use where it leads to concise, readable code. And so it should be with .

---

**Use case 1: The “swap-and-iterate” pattern**

 This pattern occurs a lot in event-driven architectures; one might typically have a vector of events to dispatch or, equivalently, callbacks to invoke. But we want event handlers to be able to produce events of their own for deferred dispatch.

```cpp
    void process() {
        std::vector<Callback> tmp{};
        using std::swap; // the "std::swap" two-step
        std::swap(tmp, callbacks_);
        for (const auto& callback : tmp) {
            std::invoke(callback);
        }
    }
```

- doing above, we’re also guilty of incurring the “ITM antipattern” [4]. First, we construct an empty vector (`tmp`), then — with `swap` — we have 3 move assignments before we get to the business of iterating.
- Refactoring with std::exchange solves these problems:

```cpp
    void process() {
        for (const auto& callback : std::exchange(callbacks_, {}) {
            std::invoke(callback);
        }
    }
```

- Now we don’t have to declare a temporary. Inside std::exchange we have one move construction and one move assignment, saving one move compared to swap.
-  compiler is really good at optimising the call to std::exchange, so of course we get the copy elision we would normally expect. As a result, the code overall is more concise, faster, and provides the same safety as before.

---

**Use case 2: Posting to another thread**

- A similar pattern occurs in any multithreaded setting where we want to capturean object in a lambda expression and post it to another thread. std::exchange allows us to efficiently transfer ownership of an object’s “guts.”

```cpp
    void post_event(Callback& cb) {
        Callback tmp{};
        using std::swap;
        swap(cb, tmp);
        PostToMainThread([this, cb_ = std::move(tmp)] {
            callbacks_.push_back(cb_);
        });
    }
```

- Here we are taking ownership of the passed-in callback by swapping it into a temporary, then capturing that temporary in a lambda closure. We are capturing by move in an attempt to improve performance, but ultimately we’re still doing a lot more than we need to.

```cpp
    void post_event(Callback& cb) {
        PostToMainThread([this, cb_ = std::exchange(cb, {})] {
            callbacks_.push_back(cb_);
        });
    }
```

- `std::exchange` uses one move fewer then `std::swap`, and **copy elision**, a.k.a. the return value optimization, constructs the return value directly into the lambda expression’s closure.

**Why not just move?**

```cpp
    void post_event(Callback& cb) {
        PostToMainThread([this, cb_ = std::move(cb)] {
            callbacks_.push_back(cb_);
        });
    }
```

- The answer is to ensure future maintainability and flexibility. It may well be true that a moved-from Callback is considered just as empty as if we had explicitly emptied it with std::exchange, but is that obvious? Is it always going to be true? Will we ever need to update that assumption — or this code — if we change the type of Callback later on?
- In the major STL implementations, it is currently the case that a moved-from container is empty. More specifically, sequenced containers like std::vector; associative containers like std::unordered_map; and other “containers” like std::string or std::function are empty after move, even when they are small-buffer-optimised
- But this is not necessarily true of every single container type we might use. There is no particular reason why a homegrown small-buffer-optimised vector should be empty after we move from it. We find a notable standard counterexample of the “normal” behaviour in std::optional, which is still engaged after being moved from. So yes, using std::move — obviously — only incurs one move, whereas std::exchange incurs two, but at the cost of abstraction leakage. Using only std::move, we need to know and be able to reason about the move-related properties of the container we use; future maintainers (usually ourselves, in 6 months’ time) also need to know about that “empty after move” constraint on the code, which is not explicitly expressed anywhere and not obvious from inspection.
- For this reason, I recommend being explicit about clearing objects that are supposed to be empty, and std::exchange can do just that.
- In fact [cppreference.com](https://en.cppreference.com/w/cpp/utility/exchange) notes a primary use case for std::exchange in writing the move special member functions to leave the moved-from object cleared.'

**Can we use std::exchange with locks?**

- it may seem at first that std::exchange isn’t a great option when we need to access something under mutex protection:

```cpp
    void process() {
        std::vector<Callback> tmp{};
        {
            using std::swap;
            std::scoped_lock lock{mutex_};
            swap(tmp, callbacks_);
        }
        for (const auto& callback : tmp) {
            std::invoke(callback);
        }
    }
```

- Here, the vector of callbacks is protected by a mutex. We cannot afford to hold this lock while iterating, because any event handler that wants to generate an event will try to lock the mutex in order to queue its event.
- So we can’t use our std::exchange pattern naively:

```cpp
    void process() {
        std::scoped_lock lock{mutex_};
        for (const auto& callback : std::exchange(callbacks_, {})) {
            std::invoke(callback);
        }
    }
```

- because that would break our ability to queue events from callbacks. The solution, as is so often the case, is to use a function. In this instance, an immediately-invoked lambda expression fits the bill nicely.

```cpp
    // All events are dispatched when we call process
    void process() {
        const auto tmp = [&] {
            std::scoped_lock lock{mutex_};
            return std::exchange(callbacks_, {});
        }();
        for (const auto& callback : tmp) {
            std::invoke(callback);
        }
    }
```

- We reap the benefits of holding the lock for as short a time as possible; taking advantage of return value optimization; saving a move; and concision of expression.

- We could also consider below. Here, the `scoped_lock` lives until the semicolon, and the result of the **comma operator** is the result of std::exchange, used to construct tmp. I concede that many people would recoil in horror at this use of the comma operator, but that’s a topic for another article

```cpp
    // All events are dispatched when we call process
    void process() {
        const auto tmp = (std::scoped_lock{mutex_}, std::exchange(callbacks_, {}));
        for (const auto& callback : tmp) {
            std::invoke(callback);
        }
    }
```