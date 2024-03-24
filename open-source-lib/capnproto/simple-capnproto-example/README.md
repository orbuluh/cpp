# Modified from capnproto calculator

```bash
./build/simple-server/simple-server unix:tmp.sock
./build/simple-client/simple-client unix:tmp.sock

./build/simple-server/simple-server unix:tmp.sock
./build/simple-client/simple-client localhost:12345
```
