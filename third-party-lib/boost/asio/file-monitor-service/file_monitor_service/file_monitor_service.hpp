#pragma once

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include "file_monitor_service.hpp"

#pragma once

#include <errno.h>
#include <sys/inotify.h>

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/assign.hpp>
#include <boost/noncopyable.hpp>
#include <boost/ref.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <string>

#include "file_monitor_event.hpp"

namespace services {

namespace detail {

class file_monitor_service {
 public:
  class implementation_type : private boost::asio::detail::noncopyable {
    int fd_;
    std::map<int, std::string> watched_files_;
    boost::shared_ptr<boost::asio::posix::stream_descriptor> input_;
    boost::array<char, 4096> buffer_;
    std::string buffer_str_;

    friend class file_monitor_service;
  };

  explicit file_monitor_service(boost::asio::io_service& io_service)
      : io_service_(io_service) {}

  void shutdown_service() {}

  void construct(implementation_type& impl) {
    impl.fd_ = init_fd();
    impl.input_.reset(
        new boost::asio::posix::stream_descriptor(io_service_, impl.fd_));
  }

  void destroy(implementation_type& impl) { impl.input_.reset(); }

  void add_file(implementation_type& impl, const std::string& file,
                boost::system::error_code& ec) {
    int wd = inotify_add_watch(impl.fd_, file.c_str(), IN_ALL_EVENTS);

    if (wd == -1) {
      ec = boost::system::error_code(errno, boost::system::system_category());
    } else if (impl.watched_files_.find(wd) == impl.watched_files_.end()) {
      impl.watched_files_[wd] = file;
    }
  }

  template <typename MonHandler>
  void async_monitor(implementation_type& impl, boost::system::error_code& ec,
                     MonHandler handler) {
    impl.input_->async_read_some(boost::asio::buffer(impl.buffer_), [&]() {
      handle_monitor<MonHandler>(impl, boost::asio::placeholders::error,
                                 boost::asio::placeholders::bytes_transferred,
                                 handler);
    });
  }

 private:
  template <typename MonHandler>
  void handle_monitor(implementation_type& impl, boost::system::error_code ec,
                      std::size_t bytes_transferred, MonHandler handler) {
    if (!ec) {
      impl.buffer_str_.append(impl.buffer_.data(),
                              impl.buffer_.data() + bytes_transferred);

      while (impl.buffer_str_.size() >= sizeof(inotify_event)) {
        const inotify_event* iev =
            reinterpret_cast<const inotify_event*>(impl.buffer_str_.data());
        events_t::const_iterator event_i = events.find(iev->mask);
        if (event_i != events.end()) {
          io_service_.post(boost::asio::detail::bind_handler(
              handler, ec,
              event(impl.watched_files_[iev->wd], event_i->second)));
        }
        impl.buffer_str_.erase(0, sizeof(inotify_event) + iev->len);
      }
      async_monitor(impl, ec, handler);
    }
  }

  int init_fd() {
    int fd = inotify_init1(IN_NONBLOCK);
    if (fd == -1) {
      boost::system::system_error e(
          boost::system::error_code(errno, boost::system::system_category()),
          "file_monitor_service::init_fd: init_inotify failed");
      boost::throw_exception(e);
    }
    return fd;
  }

  boost::asio::io_service& io_service_;

  using events_t = std::map<int, event::type>;
  static const events_t events;
};

const file_monitor_service::events_t file_monitor_service::events = {
    {IN_ACCESS, event::type::access},
    {IN_ATTRIB, event::type::attrib},
    {IN_CLOSE_WRITE, event::type::close_write},
    {IN_CLOSE_NOWRITE, event::type::close_nowrite},
    {IN_MODIFY, event::type::modify},
    {IN_DELETE_SELF, event::type::delete_self},
    {IN_MOVE_SELF, event::type::move_self},
    {IN_OPEN, event::type::open}};
}  // namespace detail

class file_monitor_service : public boost::asio::io_service::service {
 private:
  using service_impl_type = detail::file_monitor_service;

 public:
  using implementation_type = service_impl_type::implementation_type;

  static boost::asio::io_service::id id;

  explicit file_monitor_service(boost::asio::io_service& io_service)
      : boost::asio::io_service::service(io_service),
        service_impl_(io_service) {}

  void construct(implementation_type& impl) { service_impl_.construct(impl); }

  void destroy(implementation_type& impl) { service_impl_.destroy(impl); }

  void add_file(implementation_type& impl, const std::string& file,
                boost::system::error_code& ec) {
    service_impl_.add_file(impl, file, ec);
  }

  template <typename MonHandler>
  void async_monitor(implementation_type& impl, boost::system::error_code& ec,
                     BOOST_ASIO_MOVE_ARG(MonHandler) handler) {
    service_impl_.async_monitor(impl, ec,
                                BOOST_ASIO_MOVE_CAST(MonHandler)(handler));
  }

 private:
  void shutdown_service() { service_impl_.shutdown_service(); }

 private:
  service_impl_type service_impl_;
};

boost::asio::io_service::id file_monitor_service::id;
}  // namespace services
