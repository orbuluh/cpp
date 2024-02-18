/* Modified from: https://github.com/botaojia/chat */

#include <array>
#include <boost/asio.hpp>
#include <chrono>
#include <cstring>
#include <deque>
#include <iostream>
#include <thread>

#include "protocol.hpp"

using boost::asio::ip::tcp;

using MsgT = std::array<char, MAX_IP_PACK_SIZE>;

class ChatRoomClient {
 public:
  ChatRoomClient(const std::array<char, MAX_NICKNAME>& nickname,
                 boost::asio::io_service& io_service,
                 tcp::resolver::iterator endpoint_iterator)
      : io_service_(io_service), socket_(io_service) {
    strcpy(nickname_.data(), nickname.data());
    memset(read_msg_.data(), '\0', MAX_IP_PACK_SIZE);
    boost::asio::async_connect(
        socket_, endpoint_iterator,
        [this](const auto& error, boost::asio::ip::tcp::resolver::iterator _) {
          this->onConnect(error);
        });
  }

  void write(const MsgT& msg) {
    io_service_.post([this, &msg]() { this->writeImpl(msg); });
  }

  void close() {
    io_service_.post([this]() { this->closeImpl(); });
  }

 private:
  void onConnect(const boost::system::error_code& error) {
    if (!error) {
      boost::asio::async_write(
          socket_, boost::asio::buffer(nickname_, nickname_.size()),
          [this](const auto& error, const auto& byteTransferred) {
            this->readHandler(error);
          });
    }
  }

  void readHandler(const boost::system::error_code& error) {
    std::cout << "Received: " << read_msg_.data() << std::endl;
    if (!error) {
      boost::asio::async_read(
          socket_, boost::asio::buffer(read_msg_, read_msg_.size()),
          [this](const auto& error, const auto& byteTransferred) {
            this->readHandler(error);
          });
    } else {
      std::cout << "error detected, shutdown connection\n";
      closeImpl();
    }
  }

  void writeImpl(MsgT msg) {
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress) {
      boost::asio::async_write(
          socket_,
          boost::asio::buffer(write_msgs_.front(), write_msgs_.front().size()),
          [this](const auto& error, const auto& byteTransferred) {
            this->writeHandler(error);
          });
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
            [this](const auto& error, const auto& byteTransferred) {
              this->writeHandler(error);
            });
      }
    } else {
      closeImpl();
    }
  }

  void closeImpl() { socket_.close(); }

  boost::asio::io_service& io_service_;
  tcp::socket socket_;
  MsgT read_msg_;
  std::deque<MsgT> write_msgs_;
  std::array<char, MAX_NICKNAME> nickname_;
};

int main(int argc, char* argv[]) {
  try {
    if (argc != 4) {
      std::cerr << "Usage: chat_client <nickname> <host> <port>\n";
      return 1;
    }
    boost::asio::io_service io_service;
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(argv[2], argv[3]);
    tcp::resolver::iterator iterator = resolver.resolve(query);
    std::array<char, MAX_NICKNAME> nickname;
    strcpy(nickname.data(), argv[1]);

    ChatRoomClient cli(nickname, io_service, iterator);

    std::thread t([&io_service]() { io_service.run(); });

    MsgT msg;

    while (true) {
      memset(msg.data(), '\0', msg.size());
      if (!std::cin.getline(msg.data(),
                            MAX_IP_PACK_SIZE - PADDING - MAX_NICKNAME)) {
        std::cin.clear();  // clean up error bit and try to finish reading
      }
      cli.write(msg);
    }

    cli.close();
    t.join();
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
