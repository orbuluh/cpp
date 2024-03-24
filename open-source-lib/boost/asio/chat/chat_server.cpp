/* Modified from: https://github.com/botaojia/chat */

#include <algorithm>
#include <array>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <ctime>
#include <deque>
#include <iomanip>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>

#include "protocol.hpp"

using boost::asio::ip::tcp;
using MsgT = std::array<char, MAX_IP_PACK_SIZE>;

class IParticipant {
 public:
  virtual ~IParticipant() {}
  virtual void onMessage(MsgT& msg) = 0;
};

using ParticipantSPtr = std::shared_ptr<IParticipant>;

namespace {
std::string getTimestamp() {
  time_t t = time(0);  // get time now
  struct tm* now = localtime(&t);
  std::stringstream ss;
  ss << '[' << (now->tm_year + 1900) << '-' << std::setfill('0') << std::setw(2)
     << (now->tm_mon + 1) << '-' << std::setfill('0') << std::setw(2)
     << now->tm_mday << ' ' << std::setfill('0') << std::setw(2) << now->tm_hour
     << ":" << std::setfill('0') << std::setw(2) << now->tm_min << ":"
     << std::setfill('0') << std::setw(2) << now->tm_sec << "] ";

  return ss.str();
}

class WorkerThread {
 public:
  static void run(std::shared_ptr<boost::asio::io_service> io_service) {
    {
      std::lock_guard<std::mutex> lock(m);
      std::cout << "[" << std::this_thread::get_id() << "] Thread starts"
                << std::endl;
    }

    io_service->run();

    {
      std::lock_guard<std::mutex> lock(m);
      std::cout << "[" << std::this_thread::get_id() << "] Thread ends"
                << std::endl;
    }
  }

 private:
  static std::mutex m;
};

std::mutex WorkerThread::m;
}  // namespace

class ChatRoom {
 public:
  void enter(ParticipantSPtr participant, const std::string& nickname) {
    participants_.insert(participant);
    name_table_[participant] = nickname;
    std::for_each(recent_msgs_.begin(), recent_msgs_.end(),
                  [participant](auto& msg) { participant->onMessage(msg); });
  }

  void leave(ParticipantSPtr participant) {
    participants_.erase(participant);
    name_table_.erase(participant);
  }

  void broadcast(MsgT& msg, ParticipantSPtr participant) {
    std::string timestamp = getTimestamp();
    std::string nickname = getNickname(participant);
    MsgT formatted_msg;

    // boundary correctness is guarded by protocol.hpp
    strcpy(formatted_msg.data(), timestamp.c_str());
    strcat(formatted_msg.data(), nickname.c_str());
    strcat(formatted_msg.data(), msg.data());

    recent_msgs_.push_back(formatted_msg);
    while (recent_msgs_.size() > max_recent_msgs) {
      recent_msgs_.pop_front();
    }

    std::for_each(participants_.begin(), participants_.end(),
                  [&formatted_msg](auto& p) { p->onMessage(formatted_msg); });
  }

  std::string getNickname(ParticipantSPtr participant) {
    return name_table_[participant];
  }

 private:
  enum { max_recent_msgs = 100 };
  std::unordered_set<ParticipantSPtr> participants_;
  std::unordered_map<ParticipantSPtr, std::string> name_table_;
  std::deque<MsgT> recent_msgs_;
};

class SessionPerPerson : public IParticipant,
                         public std::enable_shared_from_this<SessionPerPerson> {
 public:
  SessionPerPerson(boost::asio::io_service& io_service,
                   boost::asio::io_service::strand& strand, ChatRoom& room)
      : socket_(io_service), strand_(strand), room_(room) {}

  tcp::socket& socket() { return socket_; }

  void start() {
    boost::asio::async_read(
        socket_, boost::asio::buffer(nickname_, nickname_.size()),
        strand_.wrap([self = shared_from_this()](const auto& error,
                                                 const auto& byteTransferred) {
          self->nicknameHandler(error);
        }));
  }

  void onMessage(MsgT& msg) {
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress) {
      boost::asio::async_write(
          socket_,
          boost::asio::buffer(write_msgs_.front(), write_msgs_.front().size()),
          strand_.wrap([self = shared_from_this()](
                           const auto& error, const auto& byteTransferred) {
            self->writeHandler(error);
          }));
    }
  }

 private:
  void nicknameHandler(const boost::system::error_code& error) {
    if (strlen(nickname_.data()) <= MAX_NICKNAME - 2) {
      strcat(nickname_.data(), ": ");
    } else {
      // cut off nickname if too long
      nickname_[MAX_NICKNAME - 2] = ':';
      nickname_[MAX_NICKNAME - 1] = ' ';
    }

    room_.enter(shared_from_this(), std::string(nickname_.data()));

    boost::asio::async_read(
        socket_, boost::asio::buffer(read_msg_, read_msg_.size()),
        strand_.wrap([self = shared_from_this()](const auto& error,
                                                 const auto& byteTransferred) {
          self->readHandler(error);
        }));
  }

  void readHandler(const boost::system::error_code& error) {
    if (!error) {
      room_.broadcast(read_msg_, shared_from_this());

      boost::asio::async_read(
          socket_, boost::asio::buffer(read_msg_, read_msg_.size()),
          strand_.wrap([self = shared_from_this()](
                           const auto& error, const auto& byteTransferred) {
            self->readHandler(error);
          }));
    } else {
      room_.leave(shared_from_this());
    }
  }

  void writeHandler(const boost::system::error_code& error) {
    if (!error) {
      write_msgs_.pop_front();

      if (!write_msgs_.empty()) {
        boost::asio::async_write(
            socket_,
            boost::asio::buffer(write_msgs_.front(),
                                write_msgs_.front().size()),
            strand_.wrap([self = shared_from_this()](
                             const auto& error, const auto& byteTransferred) {
              self->writeHandler(error);
            }));
      }
    } else {
      room_.leave(shared_from_this());
    }
  }

  tcp::socket socket_;
  boost::asio::io_service::strand& strand_;
  ChatRoom& room_;
  std::array<char, MAX_NICKNAME> nickname_;
  MsgT read_msg_;
  std::deque<MsgT> write_msgs_;
};

class ChatRoomServer {
 public:
  ChatRoomServer(boost::asio::io_service& io_service,
                 boost::asio::io_service::strand& strand,
                 const tcp::endpoint& endpoint)
      : io_service_(io_service),
        strand_(strand),
        acceptor_(io_service, endpoint) {
    run();
  }

 private:
  void run() {
    std::shared_ptr<SessionPerPerson> new_participant(
        new SessionPerPerson(io_service_, strand_, room_));
    acceptor_.async_accept(
        new_participant->socket(),
        strand_.wrap([this, new_participant](const auto& error) {
          this->onAccept(new_participant, error);
        }));
  }

  void onAccept(std::shared_ptr<SessionPerPerson> new_participant,
                const boost::system::error_code& error) {
    if (!error) {
      new_participant->start();
    }

    run();
  }

  boost::asio::io_service& io_service_;
  boost::asio::io_service::strand& strand_;
  tcp::acceptor acceptor_;
  ChatRoom room_;
};

//----------------------------------------------------------------------

int main(int argc, char* argv[]) {
  try {
    if (argc < 2) {
      std::cerr << "Usage: chat_server <port> [<port> ...]\n";
      return 1;
    }

    std::shared_ptr<boost::asio::io_service> io_service(
        new boost::asio::io_service);
    boost::shared_ptr<boost::asio::io_service::work> work(
        new boost::asio::io_service::work(*io_service));
    boost::shared_ptr<boost::asio::io_service::strand> strand(
        new boost::asio::io_service::strand(*io_service));

    std::cout << "[" << std::this_thread::get_id() << "]"
              << "server starts" << std::endl;

    std::list<ChatRoomServer> servers;
    for (int i = 1; i < argc; ++i) {
      tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[i]));
      servers.emplace_back(*io_service, *strand, endpoint);
    }

    boost::thread_group workers;
    for (int i = 0; i < 1; ++i) {
      boost::thread* t =
          new boost::thread{[io_service]() { WorkerThread::run(io_service); }};

#ifdef __linux__
      // bind cpu affinity for worker thread in linux
      cpu_set_t cpuset;
      CPU_ZERO(&cpuset);
      CPU_SET(i, &cpuset);
      pthread_setaffinity_np(t->native_handle(), sizeof(cpu_set_t), &cpuset);
#endif
      workers.add_thread(t);
    }

    workers.join_all();
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
