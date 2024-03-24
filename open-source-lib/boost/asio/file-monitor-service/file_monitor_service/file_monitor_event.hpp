#pragma once

#include <string>

namespace services {
struct event {
  enum class type {
    null = 0,
    access,
    attrib,
    close_write,
    close_nowrite,
    modify,
    delete_self,
    move_self,
    open,
  };

  type t;
  std::string filename;

  event(const std::string& f = "", type tin = event::type::null)
      : filename(f), t(tin) {}
};
}  // namespace services
