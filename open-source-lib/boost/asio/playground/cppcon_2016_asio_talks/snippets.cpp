#include "snippets.h"

#include <boost/asio.hpp>
#include <iostream>
#include <thread>

#include "../util/utility.h"

using namespace boost;

namespace cppcon_2016_asio_talks {
void snippet1() {
  asio::io_service service;
  // The deadline timer is passed the IOService as its first argument. And then
  // it's provided with, in this case, the relative time that, once it's
  // activated, how long it will take until the timer expires.
  asio::deadline_timer timer(service, posix_time::seconds(5));

  // call timer.async_wait. And at that point, that's when the timer is going to
  // begin. The async_wait is going to be passed a completion handler. This
  // completion handler is going to do nothing except print out what time it is
  // and that the timer expired.
  timer.async_wait([](auto... vn) {
    std::cout << util::get_time_now_as_string() << " : timer expired.\n";
  });

  std::cout << util::get_time_now_as_string() << " : calling run\n";

  // The `service.run()` method is called, which begins processing asynchronous
  // events. This includes waiting for the timer to expire. The role of
  // `service.run()` is analogous to the butler in the provided story, handling
  // the completion events as they occur. So each time I call run on each of
  // those threads, that is another thread that can handle completions for me.
  service.run();

  // OPverall, what's going on when we call async_wait? That request has come
  // in. It's sitting there, which is just a timer. It's sitting there waiting
  // to be done. Once it's done, it is placed on the completion queue. The next
  // available thread will pick that up and deliver it to the handler that wants
  // it, the completion handler.

  std::cout << util::get_time_now_as_string() << " : done.\n";
}

void timer_expired(std::string id) {
  // Timer expired does nothing except for print out that we've entered timer
  // expired. It waits for three seconds and then it prints that it's exiting.
  // So we start these things off. They should each go off basically about the
  // same time. So they're supposed to go off at five seconds. So each of them
  // will produce the completion.
  std::cout << util::get_time_now_as_string() << " " << id << " enter.\n";
  std::this_thread::sleep_for(std::chrono::seconds(3));
  std::cout << util::get_time_now_as_string() << " " << id << " leave.\n";
}

void snippet2() {
  asio::io_service service;
  // create two different timers. Both are going to be five seconds. We're going
  // to start an async wait on each of them. And we will have one thread that
  // we're going to start off that is going to call run on the IO service.
  asio::deadline_timer timer1(service, posix_time::seconds(5));
  asio::deadline_timer timer2(service, posix_time::seconds(5));
  timer1.async_wait([](auto... vn) { timer_expired("timer1"); });
  timer2.async_wait([](auto... vn) { timer_expired("timer2"); });

  // 2 timer_expired() 'll be stuck on the completion queue. There's only one
  // thread. So the thread will have to handle one at a time, whichever one ends
  // up in the queue first will be handled. And then the next one. There's
  // something else in the completion queue that's able to pick up and then take
  // care of that handler. This is that idea of the butler can only deliver one
  // slushy at a time.

  // It looks like service.run is expecting that there is work to do, is the
  // basic comment. And yes, it's looking that there is work to do. How does it
  // know when it's all done? When there's nothing left inside of, you can think
  // of it as a slushy shack. When there's nothing left inside of the proactor
  // to do, there's no more work inside of the to-be-done work, there's nothing
  // inside of the completion queue, then the service.run() will return.
  std::thread butler([&]() { service.run(); });
  butler.join();
  std::cout << "done." << std::endl;
}

void snippet3() {
  // This used to show intervened result because 2 separate threads are running
  // the service.run(), but for newer boost (>= ?), it's fine.
  asio::io_service service;
  asio::deadline_timer timer1(service, posix_time::seconds(5));
  asio::deadline_timer timer2(service, posix_time::seconds(5));
  timer1.async_wait([](auto... vn) { timer_expired("timer1"); });
  timer2.async_wait([](auto... vn) { timer_expired("timer2"); });
  std::thread ta([&]() { service.run(); });
  std::thread tb([&]() { service.run(); });
  ta.join();
  tb.join();
  std::cout << "done." << std::endl;
}

void snippet4() {
  asio::io_service service;
  //  We can just say, this is equivalent of the owner just placing items
  //  directly, there's nothing to be done. There's no IO to fetch, there's no
  //  timer, there's nothing associated with it. We're just going to take a
  //  handler and we're going to stick it on the completion queue for the next
  //  thread to pick up. We are going to do this by using post.
  service.post([] { std::cout << "eat\n"; });
  service.post([] { std::cout << "drink\n"; });
  service.post([] { std::cout << "and be merry!\n"; });
  std::thread butler([&] { service.run(); });
  butler.join();
  // So this in essence is a thread queue for us, In fact, a very common pattern
  // when you're writing code using the asynchronous IO library is you will have
  // a layer that is your communication layer and it has an IO service. And then
  // you have another IO service that will be taking care of all the heavy
  // lifting processing and the two layers do nothing more than pass off work
  // between one another. So as communication comes in, you can delegate the
  // number of threads you might need for your communications and then you pass
  // that off to another IO service and those items get posted and you now have
  // the work queue in which you can provide as many threads as you need to
  // actually perform the work that you want to get done.
  std::cout << "done." << std::endl;
}

void snippet5() {
  // improved of snippet3

  // And so the code now has changed slightly. We've created, out of the IO
  // service has this thing called a strand.
  // We've taken and created a strand object. We pass it the service. These are
  // the key bits here that we've added. So we've got a strand, and now instead
  // of just calling async wait and giving it the completion handler, what I
  // want you to run when that happens, I actually take the strand and I wrap my
  // completion handler. The strand, will ensure that there is only one
  // completion handler that is wrapped by the same strand will run at the same
  // time. And that will take care of any of the threading issues that I might
  // have. So I might have lots of threads, and I might have

  // lots of things on the completion queue, items on the completion queue that
  // need to be called, but those handlers will not get called if they are
  // wrapped by the same strand.

  asio::io_context service;

  auto strand = asio::make_strand(service.get_executor());  // !!!
  asio::deadline_timer timer1(service, posix_time::seconds(5));
  asio::deadline_timer timer2(service, posix_time::seconds(5));
  timer1.async_wait(asio::bind_executor(
      strand, [](auto... vn) { timer_expired("timer1"); }));  // !!!
  timer2.async_wait(asio::bind_executor(
      strand, [](auto... vn) { timer_expired("timer2"); }));  // !!!
  std::thread ta([&]() { service.run(); });
  std::thread tb([&]() { service.run(); });
  ta.join();
  tb.join();
  std::cout << "done.\n";
}

void snippet6() {
  // Let's add a third timer for six seconds. And when we start its async_wait,
  // we will not wrap it inside the strand. So timer number one and timer number
  // two are five seconds. They're wrapped in the same strand. They're gonna go
  // off at the same time. Timer number three is six seconds. It's not wrapped
  // inside the strand. And so what would we expect to see? Well, we expect to
  // see that one and two are serialized in whichever order. Order is
  // non-deterministic. So whichever order it occurs first inside the completion
  // queue, and they get picked up. And we would expect to see, because there's
  // another thread hanging out, that timer number three will go ahead and be
  // invoked. And we do see that. So timer one goes off. And then a second
  // later, we can see that timer three will be entered. Timer one finally
  // leaves, and timer two can begin. And then our timer three is finally
  // leaving, and then timer two.

  asio::io_context service;

  auto strand = asio::make_strand(service.get_executor());
  asio::deadline_timer timer1(service, posix_time::seconds(5));
  asio::deadline_timer timer2(service, posix_time::seconds(5));
  asio::deadline_timer timer3(service, posix_time::seconds(6));

  timer1.async_wait(asio::bind_executor(
      strand, [](auto... vn) { timer_expired("timer1"); }));  // !!!
  timer2.async_wait(asio::bind_executor(
      strand, [](auto... vn) { timer_expired("timer2"); }));  // !!!
  timer3.async_wait(
      [](auto... vn) { timer_expired("timer3"); });  // not in strand

  std::thread ta([&]() { service.run(); });
  std::thread tb([&]() { service.run(); });
  ta.join();
  tb.join();
  // So if you have things that need to be serialized, for example, in IO, we
  // can't be going around writing to a TCP socket from multiple threads at the
  // same time, right? That's a disaster. So writes, we would want to wrap
  // inside of a strand. We would want to make sure that the access to that
  // writing process is protected.
  std::cout << "done.\n";
}

}  // namespace cppcon_2016_asio_talks