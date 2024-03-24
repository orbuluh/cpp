#include <capnp/message.h>
#include <capnp/rpc-twoparty.h>
#include <kj/async-io.h>
#include <kj/debug.h>

#include <iostream>

#include "protocol.capnp.h"

class addOneImpl final : public AddOneFeature::Server {
  // Implementation of the Feature1 Cap'n Proto interface.
 public:
  kj::Promise<void> addOne(AddOneContext context) override {
    auto val = context.getParams().getSrcVal();
    auto response = context.getResults();
    response.setTgtVal(val + 1);
    std::cout << "server sending:" << response.getTgtVal() << '\n';
    return kj::READY_NOW;
  }
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

  // Start the RPC server.
  capnp::TwoPartyServer server(kj::heap<addOneImpl>());

  // Run forever, accepting connections and handling requests.
  server.listen(*listener).wait(io.waitScope);
}
