# Modified from [cap-sample](https://github.com/AngryMane/cap-sample)

## [capnproto rpc](https://capnproto.org/cxxrpc.html)

- While multiple threads are allowed, each thread must have its own event loop.
- KJ discourages fine-grained interaction between threads as synchronization is expensive and error-prone.
- threads are encouraged to communicate through Cap’n Proto RPC.

- Function calls that do I/O must do so asynchronously, and must return a “futures” for the result.
- “futures” are placeholders for the results of operations that have not yet completed. When the operation completes, we say that the promise “resolves” to a value, or is “fulfilled”. A promise can also be “rejected”, which means an exception occurred.
- If you want to do something with the result of a promise, you must first wait for it to complete. This is normally done by registering a callback to execute on completion.

```cpp
kj::Promise<kj::String> contentPromise = fetchHttp("http://example.com");

kj::Promise<int> lineCountPromise =
    promise.then([](kj::String&& content) { return countChars(content, '\n'); },
                 [](kj::Exception&& exception) {
                   // Error!  Pretend the document was empty.
                   return 0;
                 });

// then() actually returns a promise for the resolution of the latter promise –
// that is, Promise<Promise<T>> is automatically reduced to Promise<T>
```

- then() takes an optional second parameter for handling errors. Think of this like a catch block.
- Note that the KJ framework coerces all exceptions to kj::Exception – the exception’s description (as returned by what()) will be retained, but any type-specific information is lost.
- It is recommended that Cap’n Proto code use the assertion macros in `kj/debug.h` to throw exceptions rather than use the C++ throw keyword. These macros make it easy to add useful debug information to an exception and generally play nicely with the KJ framework.
- It is illegal for code running in an event callback to wait, since this would stall the event loop. However, if you are the one responsible for starting the event loop in the first place, then KJ makes it easy to say “run the event loop until this promise resolves, then return the result”.

```cpp
kj::EventLoop loop;
kj::WaitScope waitScope(loop);

kj::Promise<kj::String> contentPromise =
    fetchHttp("http://example.com");

// Note: Using wait() is common in high-level client-side code.
// On the other hand, it is almost never used in servers.
kj::String content = contentPromise.wait(waitScope);

int lineCount = countChars(content, '\n');
```

- Callbacks registered with .then() which aren’t themselves asynchronous (i.e. they return a value, not a promise) by default won’t execute unless the result is actually used – they are executed “lazily”.
- To force a .then() callback to execute as soon as its input is available, do one of the following:
  - Add it to a `kj::TaskSet` – this is usually the best choice. You can cancel all tasks in the set by destroying the TaskSet.
  - `.wait()` on it – but this only works in a top-level wait scope, typically your program’s main function.
  - Call `.eagerlyEvaluate()` on it. This returns a new Promise. You can cancel the task by destroying this Promise (without otherwise consuming it).
- The complete API is documented directly in the kj/async.h header. Additionally, see the kj/async-io.h header for APIs for performing basic network I/O

### Generated code

```cpp
// interface Directory {
//   create @0 (name :Text) -> (file :File);
//   open @1 (name :Text) -> (file :File);
//   remove @2 (name :Text);
// }

struct Directory {
  Directory() = delete;

  class Client;
  class Server;
  //-----------------------------------
  // corresponding to create @0 (name :Text) -> (file :File);
  struct CreateParams;
  struct CreateResults;
  //-----------------------------------
  // corresponding to open @1 (name :Text) -> (file :File);
  struct OpenParams;
  struct OpenResults;
  //-----------------------------------
  // corresponding to remove @2 (name :Text);
  struct RemoveParams;
  struct RemoveResults;
};
```

```cpp
// For the same interface, both class of Server and Client will be generated

// The generated Client type represents a reference to a remote Server.
// Clients are pass-by-value types that use reference counting under the hood.
// (Warning: For performance reasons, the reference counting used by Clients is
// not thread-safe, so you must not copy a Client to another thread, unless you
// do it by means of an inter-thread RPC.)

class Directory::Client : public virtual capnp::Capability::Client {
 public:

  // A Client can be implicitly constructed from any of ...

  // A kj::Own<Server>, which takes ownership of the server object and creates
  // a client that calls it.
  Client(kj::Own<Directory::Server> server);

  // A kj::Promise<Client>, which creates a client whose methods first wait for
  // the promise to resolve, then forward the call to the resulting client.
  Client(kj::Promise<Client> promise);

  // A kj::Exception, which creates a client whose methods always throw that
  // exception.
  Client(kj::Exception exception);

  // nullptr, which creates a client whose methods always throw. This is meant
  // to be used to initialize variables that will be initialized to a real value
  // later on.
  Client(std::nullptr_t);

  // For each interface method foo(), the Client has a method fooRequest() which
  // creates a new request to call foo(). The returned capnp::Request object has
  // methods equivalent to a Builder for the parameter struct (FooParams), with
  // the addition of a method send(). send() sends the RPC and returns a
  // capnp::RemotePromise<FooResults>

  // For generic methods, the fooRequest() method will be a template; you must
  // explicitly specify type parameters.
  capnp::Request<CreateParams, CreateResults> createRequest();
  capnp::Request<OpenParams, OpenResults> openRequest();
  capnp::Request<RemoveParams, RemoveResults> removeRequest();
};
```

```cpp
Directory::Client dir = ...;

// Create a new request for the `open()` method.
auto request = dir.openRequest();
request.setName("foo");

// Send the request.
auto promise = request.send();

// Make a pipelined request.
auto promise2 = promise.getFile().getSizeRequest().send();

// Wait for the full results.
auto promise3 =
    promise2.then([](capnp::Response<File::GetSizeResults>&& response) {
      cout << "File size is: " << response.getSize() << endl;
    });
```

- This `RemotePromise` is equivalent to `kj::Promise<capnp::Response<FooResults>>`, but also has methods that allow pipelining. Namely:

  - For each interface-typed result, it has a getter method which returns a Client of that type. Calling this client will send a pipelined call to the server.
  - For each struct-typed result, it has a getter method which returns an object containing pipeline getters for that struct’s fields.
  - In other words, the `RemotePromise` effectively implements a subset of the eventual results’ Reader interface – one that only allows access to interfaces and sub-structs.

  - The `RemotePromise` eventually resolves to `capnp::Response<FooResults>`, which behaves like a Reader for the result struct except that it also owns the result message.

```cpp
// The generated Server type is an abstract interface which may be subclassed to
// implement a capability. Each method takes a context argument and returns a
// kj::Promise<void> which resolves when the call is finished. The parameter and
// result structures are accessed through the context –
//    context.getParams() returns a Reader for the parameters, and
//    context.getResults() returns a Builder for the results.
// The context also has methods for controlling RPC logistics, such as
// cancellation – see capnp::CallContext in capnp/capability.h for details.

// Accessing the results through the context (rather than by returning them) is
// unintuitive, but necessary because the underlying RPC transport needs to have
// control over where the results are allocated. For example, a zero-copy shared
// memory transport would need to allocate the results in the shared memory
// segment. Hence, the method implementation cannot just create its own
// MessageBuilder.
class Directory::Server : public virtual capnp::Capability::Server {
 protected:
  typedef capnp::CallContext<CreateParams, CreateResults> CreateContext;
  typedef capnp::CallContext<OpenParams, OpenResults> OpenContext;
  typedef capnp::CallContext<RemoveParams, RemoveResults> RemoveContext;

  // server side has [fnc_from_interface] signature function name
  virtual kj::Promise<void> create(CreateContext context);
  virtual kj::Promise<void> open(OpenContext context);
  virtual kj::Promise<void> remove(RemoveContext context);
};
```

```cpp
class DirectoryImpl final : public Directory::Server {
 public:
  kj::Promise<void> open(OpenContext context) override {
    auto iter = files.find(context.getParams().getName());

    // Throw an exception if not found.
    KJ_REQUIRE(iter != files.end(), "File not found.");

    context.getResults().setFile(iter->second);

    return kj::READY_NOW;
  }

  // Any method which we don't implement will simply throw
  // an exception by default.

 private:
  std::map<kj::StringPtr, File::Client> files;
};
```

- On the server side, generic methods are NOT templates. Instead, the generated code is exactly as if all of the generic parameters were bound to `AnyPointer`.
- The server generally does not get to know exactly what type the client requested; it must be designed to be correct for any parameterization.

### Initializing RPC

- Cap’n Proto makes it easy to start up an RPC client or server using the “EZ RPC” classes, defined in capnp/ez-rpc.h.
- These classes get you up and running quickly, but they hide a lot of details that power users will likely want to manipulate. Check out the comments in ez-rpc.h to understand exactly what you get and what you miss.

```cpp
// Client code

#include <capnp/ez-rpc.h>

#include <iostream>

#include "my-interface.capnp.h"

int main(int argc, const char* argv[]) {
  // We expect one argument specifying the server address.
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " HOST[:PORT]" << std::endl;
    return 1;
  }

  // Set up the EzRpcClient, connecting to the server on port
  // 5923 unless a different port was specified by the user.
  capnp::EzRpcClient client(argv[1], 5923);
  auto& waitScope = client.getWaitScope();

  // Request the bootstrap capability from the server.
  MyInterface::Client cap = client.getMain<MyInterface>();

  // Make a call to the capability.
  auto request = cap.fooRequest();
  request.setParam(123);
  auto promise = request.send();

  // Wait for the result.  This is the only line that blocks.
  auto response = promise.wait(waitScope);

  // All done.
  std::cout << response.getResult() << std::endl;
  return 0;
}

// Note that for the connect address, Cap’n Proto supports DNS host names as
// well as IPv4 and IPv6 addresses. Additionally, a Unix domain socket can be
// specified as unix: followed by a path name, and an abstract Unix domain
// socket can be specified as unix-abstract: followed by an identifier.
```

```cpp
// server:

#include <capnp/ez-rpc.h>

#include <iostream>

#include "my-interface-impl.h"

int main(int argc, const char* argv[]) {
  // We expect one argument specifying the address to which
  // to bind and accept connections.
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " ADDRESS[:PORT]" << std::endl;
    return 1;
  }

  // Set up the EzRpcServer, binding to port 5923 unless a
  // different port was specified by the user.  Note that the
  // first parameter here can be any "Client" object or anything
  // that can implicitly cast to a "Client" object.  You can even
  // re-export a capability imported from another server.
  capnp::EzRpcServer server(kj::heap<MyInterfaceImpl>(), argv[1], 5923);
  auto& waitScope = server.getWaitScope();

  // Run forever, accepting connections and handling requests.
  kj::NEVER_DONE.wait(waitScope);
}
```
