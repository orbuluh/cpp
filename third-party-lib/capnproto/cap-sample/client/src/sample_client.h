#include <capnp/rpc-twoparty.h>
#include <unistd.h>

#include <iostream>
#include <thread>
#include <vector>

#include "sample.capnp.h"

class SampleClient {
 private:
  class Server2ClientEvent : public Sample::Subscriber::Server {
   protected:
    kj::Promise<void> pushMessage(PushMessageContext context) override;
  };

 public:
  SampleClient();
  ~SampleClient();

  void start();
  void initialize();
  void subscribe(kj::AsyncIoContext& async_io,
                 kj::Own<capnp::TwoPartyClient>& rpc_client);
  bool is_running();

 private:
  bool m_IsRunning;

  std::thread m_ReceiveThread;

  kj::Own<kj::NetworkAddress> m_Address;

  kj::Own<kj::AsyncIoStream> m_Connection;

  kj::AsyncIoContext m_SendAsyncIoContext;
  kj::Own<capnp::TwoPartyClient> m_SendRPC;
  kj::Own<capnp::TwoPartyClient> m_ReceiveRPC;
  Sample::Subscriber::Client m_SubscriberImpl;

  // This keeps the response instance of the subscribe requests.
  // kj Server can't call back subscriber when the client app releases the
  // responses in this.
  std::vector<capnp::Response<Sample::SubscribeResults>> m_SubscriberLifeCycle;
};
