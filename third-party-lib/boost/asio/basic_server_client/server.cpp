#include <boost/asio.hpp>
#include <iostream>
#include <memory>

/*
A strand is a synchronization mechanism within the io_context that ensures
serialized execution of handlers, providing control over the concurrency of
handlers within the context. It helps guarantee sequential execution of certain
operations while allowing other parts of the application to execute concurrently
*/

/*
Note: The use of shared_from_this() and capturing self in the lambda function
ensures that the Session object remains valid until the completion of the
asynchronous operation.

Inside the lambda function, there is a callback for the asynchronous write
operation. This lambda function might be invoked after the Session object has
been destroyed if the Session object is managed by a raw pointer.

By using shared_from_this(), a std::shared_ptr is created that shares
ownership of the Session object. Capturing self in the lambda function ensures
that the std::shared_ptr remains alive throughout the execution of the lambda.

By extending the lifetime of the Session object using shared_from_this(), we
can safely access member variables and call member functions inside the lambda
without the risk of accessing invalid memory. This allows the asynchronous
operations to complete successfully and ensures proper cleanup and destruction
of the Session object when it's no longer needed.
*/

class Session : public std::enable_shared_from_this<Session> {
 public:
  Session(boost::asio::ip::tcp::socket socket,
          boost::asio::io_context::strand &strand)
      : socket_(std::move(socket)), strand_(strand) {}

  void start() { read(); }

 private:
  void read() {
    std::cout << "server async read...\n";
    boost::asio::async_read_until(
        socket_, buffer_, '\n',
        boost::asio::bind_executor(
            strand_,
            [self = shared_from_this()](const boost::system::error_code &error,
                                        std::size_t bytesTransferred) {
              if (!error) {
                auto start = boost::asio::buffers_begin(self->buffer_.data());
                std::string message(start, start + bytesTransferred);
                assert(message.back() == '\n');
                message.pop_back();  // r_strip the \n
                std::cout << "Received message: \"" << message << "\" from " <<
                  self->socket_.remote_endpoint() << '\n';
                self->write("Server ack'ed message: " + message);
              } else if (error == boost::asio::error::eof) {
                // Client disconnected
                std::cout << "Client disconnected." << std::endl;
              } else {
                // Other read error
                std::cerr << "Read error: " << error.message() << std::endl;
              }
              // Clean up the buffer
              self->buffer_.consume(bytesTransferred);
            }));
  }

  void write(const std::string &message) {
    boost::asio::async_write(
        socket_, boost::asio::buffer(message + '\n'),
        boost::asio::bind_executor(
            strand_,
            [self = shared_from_this()](const boost::system::error_code &error,
                                        std::size_t /*bytesTransferred*/) {
              if (!error) {
                self->read();
              } else {
                std::cerr << "Write error: " << error.message() << std::endl;
              }
            }));
  }

  boost::asio::ip::tcp::socket socket_;
  boost::asio::streambuf buffer_;
  boost::asio::io_context::strand strand_;
};

class Server {
 public:
  Server(boost::asio::io_context &ioContext, unsigned short port)
      : port_(port),
        acceptor_(ioContext, boost::asio::ip::tcp::endpoint(
                                 boost::asio::ip::tcp::v4(), port)),
        strand_(ioContext) {
    std::cout << "server started to listen on port=" << port_ << '\n';
    startAccept();
  }

 private:
  void startAccept() {
    acceptor_.async_accept([this](const boost::system::error_code &error,
                                  boost::asio::ip::tcp::socket socket) {
      if (!error) {
        // The std::make_shared<Session> function creates a new Session object,
        // which is allocated dynamically and managed by a shared pointer. This
        // allows the Session object to persist even after the completion
        // handler (this lambda) exits.

        boost::asio::ip::tcp::endpoint remoteEndpoint =
          socket.remote_endpoint();
        boost::asio::ip::tcp::endpoint localEndpoint =
          socket.local_endpoint();
        std::cout << "Start new session for client: " << remoteEndpoint
          << " using local: " << localEndpoint << '\n';
        std::make_shared<Session>(std::move(socket), std::ref(strand_))
            ->start();
      }

      // After handling the accepted connection, startAccept() is called again
      // to initiate the next accept operation, creating a loop that
      // continuously accepts new connections.
      startAccept();
    });
  }
  unsigned short port_ = 0;
  boost::asio::ip::tcp::acceptor acceptor_;
  boost::asio::io_context::strand strand_;
};

int main() {
  boost::asio::io_context ioContext;
  Server server(ioContext, 12345);
  ioContext.run();
  return 0;
}
