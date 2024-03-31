#include <capnp/message.h>
#include <capnp/rpc-twoparty.h>
#include <kj/async-io.h>
#include <kj/debug.h>

#include <iostream>
#include <thread>

#include "protocol.capnp.h"

class DataProvider final
    : public DataProviderInterface::Server,
      public kj::TaskSet::ErrorHandler,  // to handle fail send
      public kj::Refcounted  // as capnp::TwoPartyServer would need a reference
{
 public:
  DataProvider(kj::Own<kj::ConnectionReceiver>& listenerFromMain,
               kj::WaitScope& waitScopeFromMain)
      : listener_(listenerFromMain),
        wait_scope_(waitScopeFromMain),
        executor_(kj::getCurrentThreadExecutor()),
        sent_promises_(*this) {}

 private:
  template <typename DataT>
  auto GetDataHandle(MakeSubscriptionContext context) {
    return std::move(context.getParams()
                         .getSubscriberHandle()
                         .castAs<DataSubscriberHandle<DataT>>());
  }

 public:
  kj::Promise<void> makeSubscription(MakeSubscriptionContext context) override {
    auto data_type = context.getParams().getDataType();
    switch (data_type) {
      case DataType::FOO_DATA:
        foo_clients_.add(GetDataHandle<FooData>(context));
        break;
      case DataType::BAR_DATA:
        bar_clients_.add(GetDataHandle<BarData>(context));
        break;
      default:
        std::cout << "Unknown data subscription request\n";
    }
    return kj::READY_NOW;
  }

 public:
  void taskFailed(kj::Exception&& exception) override {
    // you can see this when you close clients
    (void)exception;
    std::cout << "taskFailed triggers ...\n";
  }

 public:
  void start() {
    // Start the RPC server.
    std::thread t([this]() {
      while (true) {
        executor_.executeSync([this]() {
          for (auto& client : foo_clients_) {
            auto event_request = client.onSubscribedDataRequest();
            event_request.getDataFromPublisher().setFooString("foo");
            sent_promises_.add(event_request.send().then(
                [](auto resp) { (void)resp; },
                [](auto&& exception) {
                  std::cout
                      << "Send failed: " << exception.getDescription().cStr()
                      << '\n';
                  std::cout << "failed to send Foo\n";
                  // rethrow to trigger taskFailed for demonstration
                  // you can also just do a subscriber clean up here
                  kj::throwRecoverableException(kj::mv(exception));
                }));
          }
          for (auto& client : bar_clients_) {
            auto event_request = client.onSubscribedDataRequest();
            event_request.getDataFromPublisher().setBarString("bar");
            sent_promises_.add(event_request.send().then(
                [](auto resp) { (void)resp; },
                [](auto&& exception) {
                  std::cout
                      << "Send failed: " << exception.getDescription().cStr()
                      << '\n';
                  std::cout << "failed to send Bar\n";
                  // rethrow to trigger taskFailed for demonstration
                  // you can also just do a subscriber clean up here
                  kj::throwRecoverableException(kj::mv(exception));
                }));
          }
        });

        using std::chrono_literals::operator""s;
        std::this_thread::sleep_for(1s);
      };
    });

    capnp::TwoPartyServer server(kj::addRef(*this));
    // Run forever, accepting connections and handling requests.
    server.listen(*listener_).wait(wait_scope_);
  }

 private:
  kj::Own<kj::ConnectionReceiver>& listener_;
  kj::Vector<DataSubscriberHandle<FooData>::Client> foo_clients_;
  kj::Vector<DataSubscriberHandle<BarData>::Client> bar_clients_;
  kj::WaitScope& wait_scope_;
  kj::TaskSet sent_promises_;

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
  auto data_provider = kj::refcounted<DataProvider>(listener, io.waitScope);
  data_provider->start();
}
