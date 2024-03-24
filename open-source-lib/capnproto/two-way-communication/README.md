# "Two way" communication though publisher/subscriber

- Note: the a-ha moment of kj is the implication of "server" and "client". A server is someone who provides a "capability" to do something. A client is someone who request the server's capability handle, so that it can do the "remote procedure call" on that capability.
- So to realize "two way" communication, the correct way is that, for both 2 services, they all need each other's handle, so that each other can launch the send request.

## Quick usage

```bash
./build/simple-server/simple-server unix:tmp.sock
./build/simple-client/simple-client unix:tmp.sock

./build/simple-server/simple-server localhost:12345
./build/simple-client/simple-client localhost:12345
```
