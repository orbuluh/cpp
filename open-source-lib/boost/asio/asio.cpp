#include <boost/asio.hpp>
#include <iostream>
#include <vector>

#include "../cookbook.h"

namespace boost_asio {

void sync_timer() {
  // https://think-async.com/Asio/asio-1.24.0/doc/asio/tutorial/tuttimer1/src.html
  boost::asio::io_context io;
  std::cout << "Sync timer demo\nCount down 5 seconds...\n";

  boost::asio::steady_timer t(io, boost::asio::chrono::seconds(5));
  t.wait();

  std::cout << "Hello, world!\n";
}

void demo() {
  sync_timer();
}

}  // namespace boost_asio