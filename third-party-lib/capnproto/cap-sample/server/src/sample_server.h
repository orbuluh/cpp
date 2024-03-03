
#include <kj/async-io.h>

#include "rpc_event.h"

class SampleServer : public Server2ClientEvent {
 public:
  SampleServer();
  void start(std::string server_adder);

 public:
  virtual void taskFailed(kj::Exception&& exception);

 public:
  virtual void push_message_request();

 private:
  kj::Own<RPCServer> m_RPCServerImpl;
  kj::Maybe<const kj::Executor&> m_AsyncExecutor;
  kj::AsyncIoContext m_AsynIoContext;
};
