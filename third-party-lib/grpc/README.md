# GRPC

## Lib installation side note

- Need to make sure proto version is compatible with grpc
- Build from source

```bash
git clone https://github.com/grpc/grpc
cd grpc/
git submodule update --init
mkdir -p cmake/build
cd cmake/build
cmake -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release ../..
make -j$(nproc)
sudo make install
sudo ldconfig
```

- Running above should help you to find those needed protobuf compiler in place like `/usr/local/bin/protoc`.
- However, if you have used `sudo apt install libprotobuf-dev protobuf-compiler libgrpc++-dev libgrpc-dev` or so, you will find something under `/usr/bin/protoc` which could have an incompatible version of proto compiler.
- If it's the case `sudo apt remove` them, then when running `whereis protoc`, you should see the one left should look compatible with the result of `whereis grpc_cpp_plugin`, e.g. should under `/usr/local/bin/grpc_cpp_plugin`

