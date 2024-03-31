# "Two way" communication though publisher/subscriber

- Note: the a-ha moment of kj is the implication of "SomeGeneratedType::Server" and "SomeGeneratedType::Client".
- A SomeGeneratedType::Server is someone who provides a "capability" to do something. A SomeGeneratedType::Client is someone who request the server's capability handle, so that it can do the "remote procedure call" on that capability.
- So in a subscriber/publisher scenario, it will be like: subscriber ask publisher's capability to register itself. Then publisher use subscriber's capability to invoke "on data handling"
- On the other hand, for TwoParty"Server" and TwoParty"Client" - server is the one who listens on connections. And client is the one who make the connection request!

## Quick usage

```bash
./build/simple-server/simple-server unix:tmp.sock
./build/simple-client/simple-client unix:tmp.sock

./build/simple-server/simple-server localhost:12345
./build/simple-client/simple-client localhost:12345
```
