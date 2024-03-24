#include <capnp/message.h>
#include <capnp/rpc-twoparty.h>
#include <kj/async-io.h>
#include <kj/debug.h>

#include <iostream>
#include <thread>

#include "protocol.capnp.h"

class SampleService final : public ServiceCommunication::Server,
                            public kj::Refcounted {
 public:
  SampleService(kj::WaitScope& waitScopeFromMain,
                kj::Own<kj::ConnectionReceiver>& listenerFromMain)
      : waitScope_(waitScopeFromMain),
        listener_(listenerFromMain),
        executor_(kj::getCurrentThreadExecutor()) {}

  kj::Promise<void> makeSubscription(MakeSubscriptionContext context) override {
    clients_.add(context.getParams().getClientHandle());
    std::cout << "received subscription\n";
    return kj::READY_NOW;
  }

  void start() {
    // Start the RPC server.
    std::thread t([this]() {
      while (true) {
        executor_.executeSync([&]() {
          for (auto& client : clients_) {
            auto event_request = client.onServiceEventRequest();
            event_request.getEvent().setSomeString("blbblaa");
            auto event_promise = event_request.send();
            // event_promise.wait(waitScope_);
          }
        });

        using std::chrono_literals::operator""s;
        std::this_thread::sleep_for(1s);
      };
    });

    capnp::TwoPartyServer server(kj::addRef(*this));
    // Run forever, accepting connections and handling requests.
    server.listen(*listener_).wait(waitScope_);
    t.join();
  }

 private:
  kj::Vector<ClientHandle::Client> clients_;
  kj::WaitScope& waitScope_;
  kj::Own<kj::ConnectionReceiver>& listener_;
  const kj::Executor& executor_;
};

int main(int argc, const char* argv[]) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0]
              << " ADDRESS[:PORT]\n"
                 "Runs the server bound to the given address/port.\n"
                 "ADDRESS may be '*' to bind to all local addresses.\n"
                 ":PORT may be omitted to choose a port automatically."
              << std::endl;
    return 1;
  }

  // First we need to set up the KJ async event loop. This should happen one
  // per thread that needs to perform RPC.
  auto io = kj::setupAsyncIo();

  // Using KJ APIs, let's parse our network address and listen on it.
  kj::Network& network = io.provider->getNetwork();
  kj::Own<kj::NetworkAddress> addr =
      network.parseAddress(argv[1]).wait(io.waitScope);
  kj::Own<kj::ConnectionReceiver> listener = addr->listen();

  // Write the port number to stdout, in case it was chosen automatically.
  uint32_t port = listener->getPort();
  if (port == 0U) {
    // The address format "unix:/path/to/socket" opens a unix domain socket,
    // in which case the port will be zero.
    std::cout << "Listening on Unix socket..." << std::endl;
  } else {
    std::cout << "Listening on port " << port << "..." << std::endl;
  }
  auto sample_service = kj::refcounted<SampleService>(io.waitScope, listener);

  sample_service->start();
}
