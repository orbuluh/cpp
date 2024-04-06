#include <array>
#include <boost/asio.hpp>
#include <iostream>
#include <memory>

#include "common/common.h"

using boost::asio::buffer;
using boost::asio::ip::tcp;

class proxy : public std::enable_shared_from_this<proxy> {
  // this process listens to endpoint specified by arg[1], arg[2]).
  // this process is the middle man of client (whoever making connections to
  // this process) and target (endpoint specified by arg[3], arg[4])
  // the core logic is wrapped with this proxy class. Once connects, it reads
  // n byte from client, write n bytes to server. And vise versa.
  // This is the normal >= C++11 way of implementation.
 public:
  proxy(tcp::socket client)
      : client_(std::move(client)), server_(client_.get_executor()) {}

  void connect_to_server(tcp::endpoint target) {
    auto self = shared_from_this();
    server_.async_connect(target, [self](std::error_code error) {
      if (!error) {
        self->read_from_client();
        self->read_from_server();
      }
    });
  }

 private:
  void stop() {
    client_.close();
    server_.close();
  }

  void read_from_client() {
    auto self = shared_from_this();
    client_.async_read_some(buffer(data_from_client_),
                            [self](std::error_code error, std::size_t n) {
                              if (!error) {
                                self->write_to_server(n);
                              } else {
                                self->stop();
                              }
                            });
  }

  void write_to_server(std::size_t n) {
    auto self = shared_from_this();
    async_write(server_, buffer(data_from_client_, n),
                [self](std::error_code ec, std::size_t /*n*/) {
                  if (!ec) {
                    self->read_from_client();
                  } else {
                    self->stop();
                  }
                });
  }

  void read_from_server() {
    auto self = shared_from_this();
    server_.async_read_some(boost::asio::buffer(data_from_server_),
                            [self](std::error_code error, std::size_t n) {
                              if (!error) {
                                self->write_to_client(n);
                              } else {
                                self->stop();
                              }
                            });
  }

  void write_to_client(std::size_t n) {
    auto self = shared_from_this();
    async_write(client_, buffer(data_from_server_, n),
                [self](std::error_code ec, std::size_t /*n*/) {
                  if (!ec) {
                    self->read_from_server();
                  } else {
                    self->stop();
                  }
                });
  }

  tcp::socket client_;
  tcp::socket server_;
  std::array<char, 1024> data_from_client_;
  std::array<char, 1024> data_from_server_;
};

void listen(tcp::acceptor& acceptor, tcp::endpoint target) {
  acceptor.async_accept(
      [&acceptor, target](std::error_code error, tcp::socket client) {
        if (!error) {
          std::make_shared<proxy>(std::move(client))->connect_to_server(target);
        }

        listen(acceptor, target);
      });
}

int main(int argc, char* argv[]) {
  try {
    boost::asio::io_context ctx;

    auto res = ParseArguments(argc, argv, ctx);
    if (!res) {
      return 1;
    }

    auto& [listen_endpoint, target_endpoint] = res.value();

    tcp::acceptor acceptor(ctx, listen_endpoint);

    listen(acceptor, target_endpoint);

    ctx.run();
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }
}
