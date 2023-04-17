#include <grpcpp/grpcpp.h>

#include <iostream>
#include <memory>
#include <string>

#include "greeter.grpc.pb.h"

using greeter::Greeter;
using greeter::HelloRequest;
using greeter::HelloResponse;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class GreeterClient {
 public:
  GreeterClient(std::shared_ptr<Channel> channel)
      : stub_(Greeter::NewStub(channel)) {}

  std::string SayHello(const std::string& name) {
    HelloRequest request;
    request.set_name(name);

    HelloResponse response;
    ClientContext context;

    Status status = stub_->SayHello(&context, request, &response);

    if (status.ok()) {
      return response.message();
    } else {
      std::cout << "RPC failed" << std::endl;
      return "RPC failed";
    }
  }

 private:
  std::unique_ptr<Greeter::Stub> stub_;
};

int main(int argc, char** argv) {
  GreeterClient greeter(grpc::CreateChannel(
      "localhost:50051", grpc::InsecureChannelCredentials()));
  std::string user("world");
  std::string reply = greeter.SayHello(user);
  std::cout << "Received: " << reply << std::endl;
  return 0;
}
