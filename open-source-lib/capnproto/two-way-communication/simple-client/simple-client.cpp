#include <capnp/rpc-twoparty.h>
#include <kj/async-io.h>
#include <kj/debug.h>
#include <math.h>

#include <iostream>
#include <thread>

#include "protocol.capnp.h"

class EventHandler final : public ClientHandle::Server {
  // Implementation of the Feature1 Cap'n Proto interface.
 public:
  kj::Promise<void> onServiceEvent(OnServiceEventContext context) override {
    auto event = context.getParams().getEvent();
    std::cout << "client receive event: " << event.getSomeString().cStr()
              << '\n';
    return kj::READY_NOW;
  }
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
  ServiceCommunication::Client service_handle =
      client.bootstrap().castAs<ServiceCommunication>();
  auto request = service_handle.makeSubscriptionRequest();
  ClientHandle::Client client_handle(kj::heap<EventHandler>());
  request.setClientHandle(client_handle);
  auto response_promise = request.send();
  response_promise.wait(waitScope);
  kj::NEVER_DONE.wait(waitScope);
  return 0;
}
