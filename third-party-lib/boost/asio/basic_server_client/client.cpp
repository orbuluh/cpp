#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <vector>

class Client {
 public:
  Client(boost::asio::io_context& ioContext)
      : socket_(ioContext), strand_(ioContext) {}

  void start(const boost::asio::ip::tcp::endpoint& endpoint) {
    // Connect to the server
    socket_.async_connect(
        endpoint, [this](const boost::system::error_code& error) {
          if (!error) {
            // Connection successful, start reading from the server
            readFromServer();
            // Write to the server
            writeToServer("Test from client...");
            // Start the input thread
            startInputThread();
          } else {
            std::cerr << "Connection error: " << error.message() << '\n';
          }
        });
  }

 private:
  void readFromServer() {
    // Start an asynchronous read
    std::cout << "waiting server to send something...\n";
    boost::asio::async_read_until(
        socket_, buffer_, '\n',
        boost::asio::bind_executor(
            strand_, [this](const boost::system::error_code& error,
                            std::size_t bytesTransferred) {
              if (!error) {
                // Process the received response
                auto start = boost::asio::buffers_begin(buffer_.data());
                std::string response(start, start + bytesTransferred);
                assert(response.back() == '\n');
                response.pop_back(); // r_strip the \n
                std::cout << "Received response from server: \"" << response
                          << "\"\n";
              } else if (error == boost::asio::error::eof) {
                // Server disconnected
                std::cout << "Server disconnected." << '\n';
              } else {
                std::cerr << "Read error: " << error.message() << '\n';
              }
              // Error or not, we should consume the data
              buffer_.consume(bytesTransferred);
              if (!error) {
                readFromServer();
              }
            }));
  }

  void writeToServer(const std::string& message) {
    // Start an asynchronous write
    boost::asio::async_write(
        socket_, boost::asio::buffer(message + '\n'),
        boost::asio::bind_executor(
            strand_, [this, message](const boost::system::error_code& error,
                                     std::size_t bytesTransferred) {
              if (!error) {
                std::cout << "Sent message to server: \"" << message << "\"\n";
              } else {
                std::cerr << "Write error: " << error.message() << '\n';
              }
            }));
  }

  void startInputThread() {
    std::thread inputThread([this]() {
      while (!exitFlag_) {
        // Read user input
        std::string userInput;
        std::getline(std::cin, userInput);

        // Check if user wants to quit
        if (userInput == "quit") {
          // Set the exit flag to true
          exitFlag_ = true;
          // Close the socket
          socket_.close();
          break;
        }

        // Send user input to the server
        writeToServer(userInput);
      }
    });

    inputThread.detach();  // Detach the thread to run independently
  }

 private:
  boost::asio::ip::tcp::socket socket_;
  boost::asio::streambuf buffer_;
  boost::asio::io_context::strand strand_;
  bool exitFlag_ = false;
};

int main() {
  boost::asio::io_context ioContext;

  Client client(ioContext);
  boost::asio::ip::tcp::endpoint endpoint(
      boost::asio::ip::address::from_string("127.0.0.1"), 12345);
  client.start(endpoint);

  // Run the event loop
  ioContext.run();

  return 0;
}
