#pragma once

#include <iostream>
#include <mutex>
#include <sstream>

namespace demo_util {

class ts_cout : public std::ostringstream {
  // from https://stackoverflow.com/a/41602842/4924135
  // C++20, you could have use: std::osyncstream(std::cout)
 public:
  ts_cout() = default;
  ~ts_cout() {
    std::lock_guard<std::mutex> guard(m_);
    std::cout << this->str();
  }

 private:
  static std::mutex m_;
};

std::mutex ts_cout::m_;

}  // namespace demo_util
