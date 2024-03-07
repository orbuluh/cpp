#pragma once

#include <kj/async-io.h>
#include <unistd.h>

#include <string_view>

#include "sample.capnp.h"

class SampleServer : public Sample::Server, public kj::Refcounted {
 public:
  SampleServer();
  void start(std::string_view server_adder);

 public:
  // from Sample::Server
  kj::Promise<void> initialize(InitializeContext context);
  kj::Promise<void> subscribe(SubscribeContext context);

 public:
  void broadcastEvents();

 private:
  void broadcastEventsImpl();

 private:
  kj::Vector<Sample::Subscriber::Client> subscribed_clients_;
  kj::Maybe<const kj::Executor&> asyncExecutor_;
  kj::AsyncIoContext asyncIoContext_;
};
