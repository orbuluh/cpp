#include <array>
#include <boost/asio.hpp>
#include <boost/asio/experimental/as_tuple.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
#include <iostream>
#include <memory>

#include "common/common.h"
using boost::asio::awaitable;
using boost::asio::buffer;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::ip::tcp;
using namespace boost::asio::experimental::awaitable_operators;
using std::chrono::steady_clock;
using namespace std::literals::chrono_literals;

constexpr auto use_nothrow_awaitable =
    boost::asio::experimental::as_tuple(boost::asio::use_awaitable);

struct proxy_state {
  proxy_state(tcp::socket client) : client(std::move(client)) {}

  tcp::socket client;
  tcp::socket server{client.get_executor()};
  steady_clock::time_point deadline;
};

using proxy_state_ptr = std::shared_ptr<proxy_state>;

awaitable<void> client_to_server(proxy_state_ptr state) {
  std::array<char, 1024> data;

  for (;;) {
    state->deadline = std::max(state->deadline, steady_clock::now() + 5s);

    auto [e1, n1] = co_await state->client.async_read_some(
        buffer(data), use_nothrow_awaitable);
    if (e1) co_return;

    auto [e2, n2] = co_await async_write(state->server, buffer(data, n1),
                                         use_nothrow_awaitable);
    if (e2) co_return;
  }
}

awaitable<void> server_to_client(proxy_state_ptr state) {
  std::array<char, 1024> data;

  for (;;) {
    state->deadline = std::max(state->deadline, steady_clock::now() + 5s);

    auto [e1, n1] = co_await state->server.async_read_some(
        buffer(data), use_nothrow_awaitable);
    if (e1) co_return;

    auto [e2, n2] = co_await async_write(state->client, buffer(data, n1),
                                         use_nothrow_awaitable);
    if (e2) co_return;
  }
}

awaitable<void> watchdog(proxy_state_ptr state) {
  boost::asio::steady_timer timer(state->client.get_executor());

  auto now = steady_clock::now();
  while (state->deadline > now) {
    timer.expires_at(state->deadline);
    co_await timer.async_wait(use_nothrow_awaitable);
    now = steady_clock::now();
  }
}

awaitable<void> proxy(tcp::socket client, tcp::endpoint target) {
  auto state = std::make_shared<proxy_state>(std::move(client));

  auto [e] =
      co_await state->server.async_connect(target, use_nothrow_awaitable);
  if (!e) {
    co_await (client_to_server(state) || server_to_client(state) ||
              watchdog(state));

    state->client.close();
    state->server.close();
  }
}

awaitable<void> listen(tcp::acceptor& acceptor, tcp::endpoint target) {
  for (;;) {
    auto [e, client] = co_await acceptor.async_accept(use_nothrow_awaitable);
    if (e) break;

    auto ex = client.get_executor();
    co_spawn(ex, proxy(std::move(client), target), detached);
  }
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

    co_spawn(ctx, listen(acceptor, target_endpoint), detached);

    ctx.run();
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }
}
