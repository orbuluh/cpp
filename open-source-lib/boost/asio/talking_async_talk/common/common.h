#ifndef BOOST_ASIO_TALK_ASYNC_EP1_COMMON_LIB_H
#define BOOST_ASIO_TALK_ASYNC_EP1_COMMON_LIB_H

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/basic_resolver.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <optional>

using EndPoint = boost::asio::ip::basic_resolver_entry<boost::asio::ip::tcp>;

std::optional<std::pair<EndPoint, EndPoint>> ParseArguments(
    int argc, char* argv[], boost::asio::io_context& ctx);

#endif
