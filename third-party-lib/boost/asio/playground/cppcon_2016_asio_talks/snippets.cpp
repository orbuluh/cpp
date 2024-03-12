#include "snippets.h"

#include <boost/asio.hpp>
#include <iostream>

#include "../util/utility.h"

using namespace boost;

namespace cppcon_2016_asio_talks {
void snippet1() {
  asio::io_service service;
  asio::deadline_timer timer(service, posix_time::seconds(5));
  timer.async_wait([](auto... vn) {
    std::cout << util::get_time_now_as_string() << " : timer expired.\n";
  });
  std::cout << util::get_time_now_as_string() << " : calling run\n";
  service.run();
  std::cout << util::get_time_now_as_string() << " : done.\n";
}
}  // namespace cppcon_2016_asio_talks