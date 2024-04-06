#include "common.h"

#include <iostream>

std::optional<std::pair<EndPoint, EndPoint>> ParseArguments(
    int argc, char* argv[], boost::asio::io_context& ctx) {
  if (argc != 5) {
    std::cerr << "Usage: proxy";
    std::cerr << " <listen_address> <listen_port>";
    std::cerr << " <target_address> <target_port>\n";
    return {};
  }
  auto listen_endpoint = *boost::asio::ip::tcp::resolver(ctx).resolve(
      argv[1], argv[2], boost::asio::ip::tcp::tcp::resolver::passive);

  auto target_endpoint =
      *boost::asio::ip::tcp::resolver(ctx).resolve(argv[3], argv[4]);
  return std::optional(
      std::make_pair(std::move(listen_endpoint), std::move(target_endpoint)));
}