# From [Talking Async Ep1](https://www.youtube.com/watch?v=icgnqFM-aY4)

- Code modified from [author's repo](https://github.com/chriskohlhoff/talking-async) and use boost asio

```bash
# It's a man in the middle scenarios: when the process run ...

./build/talking_async "" 54545 www.boost.org 80


# This creates a server listening on port 54545 on localhost.
# When anyone making connection to localhost:54545, it redirects the data to
# www.boost.org:80

# You can use `telnet localhost 54545` to make a connection
# and hand-make a HTTP header to trigger the response:
telnet localhost 54545
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
GET  / HTTP/1.0 #  <---------type by yourself
```
