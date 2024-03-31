#include <capnp/rpc-twoparty.h>
#include <kj/async-io.h>
#include <kj/debug.h>
#include <math.h>

#include <functional>
#include <iostream>
#include <thread>

#include "protocol.capnp.h"

template <typename DataT>
class OnDataHandler final : public DataSubscriberHandle<DataT>::Server,
                            public kj::Refcounted {
 public:
  using RpcContext =
      typename DataSubscriberHandle<DataT>::Server::OnSubscribedDataContext;
  using DataHandlerT = std::function<void(RpcContext)>;
  OnDataHandler(DataHandlerT&& callback)
      : on_data_callback_(std::move(callback)) {}

 public:
  kj::Promise<void> onSubscribedData(RpcContext context) override {
    on_data_callback_(context);
    return kj::READY_NOW;
  }

 public:
  DataHandlerT on_data_callback_;
};

int main(int argc, const char* argv[]) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0]
              << " HOST:PORT\n"
                 "Connects to the Calculator server at the given address and "
                 "does some RPCs."
              << std::endl;
    return 1;
  }

  // First we need to set up the KJ async event loop. This should happen one
  // per thread that needs to perform RPC.
  auto io = kj::setupAsyncIo();

  // Keep an eye on `waitScope`.  Whenever you see it used is a place where we
  // stop and wait for the server to respond.  If a line of code does not use
  // `waitScope`, then it does not block!
  auto& waitScope = io.waitScope;

  // Using KJ APIs, let's parse our network address and connect to it.
  kj::Network& network = io.provider->getNetwork();
  kj::Own<kj::NetworkAddress> addr =
      network.parseAddress(argv[1]).wait(waitScope);
  kj::Own<kj::AsyncIoStream> conn = addr->connect().wait(waitScope);

  // Now we can start the Cap'n Proto RPC system on this connection.
  capnp::TwoPartyClient client(*conn);

  // The server exports a "bootstrap" capability implementing the
  // `AddOneFeature` interface.
  DataProviderInterface::Client service_handle =
      client.bootstrap().castAs<DataProviderInterface>();

  //
  auto foo_request = service_handle.makeSubscriptionRequest<FooData>();
  foo_request.setDataType(DataType::FOO_DATA);

  auto on_foo_data_handler =
      kj::refcounted<OnDataHandler<FooData>>([](auto rpc_context) {
        auto event = rpc_context.getParams().getDataFromPublisher();
        std::cout << "client receive event: " << event.getFooString().cStr()
                  << '\n';
      });
  typename DataSubscriberHandle<FooData>::Client self_foo_handle(
      kj::addRef(*on_foo_data_handler));
  foo_request.setSubscriberHandle(self_foo_handle);

  auto foo_response_promise = foo_request.send();
  foo_response_promise.wait(waitScope);
  //
  auto bar_request = service_handle.makeSubscriptionRequest<BarData>();
  bar_request.setDataType(DataType::BAR_DATA);

  auto on_bar_data_handler =
      kj::refcounted<OnDataHandler<BarData>>([](auto rpc_context) {
        auto event = rpc_context.getParams().getDataFromPublisher();
        std::cout << "client receive event: " << event.getBarString().cStr()
                  << '\n';
      });
  typename DataSubscriberHandle<BarData>::Client self_bar_handle(
      kj::addRef(*on_bar_data_handler));
  bar_request.setSubscriberHandle(self_bar_handle);

  auto bar_response_promise = bar_request.send();
  bar_response_promise.wait(waitScope);

  //---------------------------------------------------------------
  kj::NEVER_DONE.wait(waitScope);
  return 0;
}
