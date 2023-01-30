#include <coroutine>
#include <iostream>
#include <thread>

namespace coroutine_between_threads {

int id_suffix(const std::thread::id& tid) {
  return std::hash<std::thread::id>()(tid) % 1000;
}

struct awaitable {
  std::jthread& threadRef;
  awaitable(std::jthread& t) : threadRef(t) {
    std::cout << "\tawaitable constructed on thread: "
              << id_suffix(std::this_thread::get_id()) << '\n';
  }
  ~awaitable() {
    std::cout << "\tawaitable destructed on thread: "
              << id_suffix(std::this_thread::get_id())
              << " with thread id=" << id_suffix(threadRef.get_id()) << '\n';
  }
  // std required functions for coroutine
  // compiler will call them
  bool await_ready() { return false; }
  void await_resume() {}
  void await_suspend(std::coroutine_handle<> h /*compiler passed handle*/) {
    std::jthread& out = threadRef;
    std::cout << "\tawait_suspend called on tid=" << (std::hash<std::thread::id>()(std::this_thread::get_id()) % 1000) << '\n';
    out = std::jthread([h] { h.resume(); });
    std::cout << "\tawait_suspend create new thread id="
              << (std::hash<std::thread::id>()(out.get_id()) % 1000) << '\n';
  }
};

struct task {
  struct promise_type {
    task get_return_object() { return {}; }
    std::suspend_never initial_suspend() { return {}; }
    std::suspend_never final_suspend() noexcept { return {}; }
    void return_void() {}
    void unhandled_exception() {}
  };
};

task coro(std::jthread& t1, std::jthread& t2, int i) {
  std::cout << "coro() started on thread: "
            << id_suffix(std::this_thread::get_id()) << " i=" << i << '\n';
  co_await awaitable{t1};
  std::cout << "coro() resumed on thread: "
            << id_suffix(std::this_thread::get_id()) << " i=" << i << '\n';
  std::cout << "coro() continues on thread: "
            << id_suffix(std::this_thread::get_id()) << " i=" << i << '\n';
  co_await awaitable{t2};
  std::cout << "coro() resumed on thread: "
            << id_suffix(std::this_thread::get_id()) << " i=" << i << '\n';
  std::cout << "coro() done on thread: " << id_suffix(std::this_thread::get_id())
            << " i@" << &i << '\n';
}

void demo() {
  std::cout << "Main thread: " << id_suffix(std::this_thread::get_id()) << '\n';
  {
    std::jthread t1, t2;
    coro(t1, t2, 42);
    std::cout
        << "Main thread: " << id_suffix(std::this_thread::get_id())
        << " done (but jthread hasn't destroyed and coroutine still running\n";
  }
  std::cout << "Main thread: " << id_suffix(std::this_thread::get_id())
            << " really done after jthread join\n";
}
}  // namespace coroutine_between_threads