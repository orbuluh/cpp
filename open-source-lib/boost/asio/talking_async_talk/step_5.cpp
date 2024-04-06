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
namespace this_coro = boost::asio::this_coro;
using namespace boost::asio::experimental::awaitable_operators;
using std::chrono::steady_clock;
using namespace std::literals::chrono_literals;

constexpr auto use_nothrow_awaitable =
    boost::asio::experimental::as_tuple(boost::asio::use_awaitable);

awaitable<void> transfer(tcp::socket& from, tcp::socket& to,
                         steady_clock::time_point& deadline) {
  std::array<char, 1024> data;

  for (;;) {
    deadline = std::max(deadline, steady_clock::now() + 5s);

    auto [e1, n1] =
        co_await from.async_read_some(buffer(data), use_nothrow_awaitable);
    if (e1) co_return;

    auto [e2, n2] =
        co_await async_write(to, buffer(data, n1), use_nothrow_awaitable);
    if (e2) co_return;
  }
}

awaitable<void> watchdog(steady_clock::time_point& deadline) {
  boost::asio::steady_timer timer(co_await this_coro::executor);

  auto now = steady_clock::now();
  while (deadline > now) {
    timer.expires_at(deadline);
    co_await timer.async_wait(use_nothrow_awaitable);
    now = steady_clock::now();
  }
}

awaitable<void> proxy(tcp::socket client, tcp::endpoint target) {
  tcp::socket server(client.get_executor());
  steady_clock::time_point deadline{};

  auto [e] = co_await server.async_connect(target, use_nothrow_awaitable);
  if (!e) {
    co_await (transfer(client, server, deadline) ||
              transfer(server, client, deadline) || watchdog(deadline));
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
