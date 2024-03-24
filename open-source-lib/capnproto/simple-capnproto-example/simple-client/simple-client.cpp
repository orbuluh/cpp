#include <capnp/rpc-twoparty.h>
#include <kj/async-io.h>
#include <kj/debug.h>
#include <math.h>

#include <iostream>
#include <thread>

#include "protocol.capnp.h"

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
  AddOneFeature::Client client_capability =
      client.bootstrap().castAs<AddOneFeature>();

  int32_t cur_val = 1;
  while (true) {
    std::cout << "client sending:" << cur_val << '\n';
    auto request = client_capability.addOneRequest();
    request.setSrcVal(cur_val);
    auto response_promise = request.send().then(
        [&cur_val](auto result) { cur_val = result.getTgtVal() + 1; });
    response_promise.wait(waitScope);
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1s);
  }

  return 0;
}
