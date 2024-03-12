#include "utility.h"

#include <chrono>
#include <iomanip>  // put_time
#include <sstream>

namespace util {

std::string get_time_now_as_string() {
  const auto now = std::chrono::system_clock::now();
  const std::time_t t_c = std::chrono::system_clock::to_time_t(now);
  std::ostringstream os;
  os << std::put_time(std::localtime(&t_c), "%Y/%m/%d %H:%M:%S");
  return os.str();
}
}  // namespace util