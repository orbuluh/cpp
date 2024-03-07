#include "sample_server.h"

#include <capnp/rpc-twoparty.h>
#include <kj/async-io.h>
#include <kj/async-unix.h>

#include <iostream>
#include <numeric>  // for std::accumulate
#include <vector>

const static std::vector<int> TARGET_SIGNALS = {
    SIGINT,
    // SIGILL,
    SIGABRT,
    // SIGFPE,
    // SIGSEGV,
    SIGTERM, SIGHUP, SIGQUIT,
    // SIGTRAP,
    // SIGKILL,
    //  When client had gone. Cap'n Proto masks SIGPIPE, so nothing to do here.
    // SIGPIPE,
    // SIGALRM,
    // SIGURG,
    // SIGSTOP,
    // SIGTSTP,
    // SIGCONT,
    // SIGCHLD,
    //,
    // SIGTTIN,
    // SIGTTOU,
    // SIGPOLL,
    SIGXFSZ, SIGXCPU,
    // SIGVTALRM,
    // SIGPROF,
};

SampleServer::SampleServer()
    : asyncIoContext_(kj::setupAsyncIo()),
      asyncExecutor_(kj::getCurrentThreadExecutor()) {
  for (auto signal : TARGET_SIGNALS) {
    kj::UnixEventPort::captureSignal(signal);
  }
}

void SampleServer::start(std::string_view server_adder) {
  capnp::TwoPartyServer server(kj::addRef(*this));
  auto address = asyncIoContext_.provider->getNetwork()
                     .parseAddress(server_adder.data())
                     .wait(asyncIoContext_.waitScope);
  auto listener = address->listen();
  auto listenPromise = server.listen(*listener);

  kj::Promise<siginfo_t> never_done_casted =
      listenPromise.then([]() { return siginfo_t(); });
  kj::Promise<siginfo_t> on_signal =
      std::accumulate(TARGET_SIGNALS.begin(), TARGET_SIGNALS.end(),
                      kj::mv(never_done_casted),
                      [&](kj::Promise<siginfo_t>& _on_signal, int signal) {
                        return _on_signal.exclusiveJoin(
                            asyncIoContext_.unixEventPort.onSignal(signal));
                      })
          .then([](siginfo_t a) {
            std::cout << a.si_signo << std::endl;
            return a;
          });
  on_signal.wait(asyncIoContext_.waitScope);
}

void SampleServer::broadcastEvents() {
  KJ_IF_MAYBE (executor_ptr, asyncExecutor_) {
    if (!executor_ptr->isLive()) {
      std::cout << "executor does not live" << std::endl;
      return;
    }
    executor_ptr->executeSync([this]() { this->broadcastEventsImpl(); });
  }
}

kj::Promise<void> SampleServer::initialize(InitializeContext context) {
  std::cout << "[SERVER]" << __PRETTY_FUNCTION__ << " called" << std::endl;
  return kj::READY_NOW;
}

kj::Promise<void> SampleServer::subscribe(SubscribeContext context) {
  std::cout << "[SERVER]" << __PRETTY_FUNCTION__ << " called" << std::endl;
  try {
    auto subscriber = context.getParams().getSubscriber();
    subscribed_clients_.add(subscriber);
  } catch (kj::Exception e) {
    std::cout << e.getDescription().cStr() << std::endl;
  }

  return kj::READY_NOW;
}

void SampleServer::broadcastEventsImpl() {
  std::cout << "[SERVER]" << __PRETTY_FUNCTION__ << " called" << std::endl;

  // If called before running PRC server, m_RPCServerHandler has no subscribers.
  // So It's unnecessary to check running.
  for (auto subscriber : subscribed_clients_) {
    std::cout << "[SERVER]" << __PRETTY_FUNCTION__ << " called a subscriber"
              << std::endl;
    auto request = subscriber.pushMessageRequest();
    auto result = request.send();
  }
}