# Quick fact
- A credit card is a proxy for a bank account, which is a proxy for a bundle of cash. Both implement the same interface: they can be used for making a payment.
- Proxy is a structural design pattern that lets you provide a substitute or placeholder for another object.
- A proxy controls access to the original object, allowing you to perform something either before or after the request gets through to the service object. The proxy must follow this interface to be able to disguise itself as a service object.


# Proxies for different purposes
Proxy isn’t really a homogeneous API, there the different kinds of proxies people build for entirely different purposes.
- **Lazy initialization (virtual proxy)**
    - This is when you have a heavyweight service object that wastes system resources by being always up, even though you only need it from time to time. Instead of creating the object when the app launches, you can delay the object’s initialization to a time when it’s really needed.
- **Access control (protection proxy)**
    - This is when you want only specific clients to be able to use the service object
- **Local execution of a remote service (remote proxy)**
    - When the service object is located on a remote server, the proxy passes the client request over the network, handling all of the nasty details of working with the network.
- **Logging requests (logging proxy)**
    - This is when you want to keep a history of requests to the service object. The proxy can log each request before passing it to the service.
- **Caching request results (caching proxy)**
    - This is when you need to cache results of client requests and manage the life cycle of this cache, especially if results are quite large
- **Smart reference**
    - when you need to be able to dismiss a heavyweight object once there are no clients that use it.
    - From time to time, the proxy may go over the clients and check whether they are still active. If the client list gets empty, the proxy might dismiss the service object and free the underlying system resources. The proxy can also track whether the client had modified the service object. Then the unchanged objects may be reused by other clients.
    - smart pointer is a proxy as well - it keeps a reference count, overrides certain operators, but all in all, it provides you the interface that you would get in an ordinary pointer.

# Comparison
- Adapter provides a different interface to the wrapped object, Proxy provides it with the same interface, and Decorator provides it with an enhanced interface.
- Facade is similar to Proxy in that both buffer a complex entity and initialize it on its own. Unlike Facade, Proxy has the same interface as its service object, which makes them interchangeable.
- Decorator and Proxy have similar structures, but very different intents.
  - Both patterns are built on the composition principle, where one object is supposed to delegate some of the work to another.
  - The difference is that a Proxy usually manages the life cycle of its service object on its own, whereas the composition of Decorators is always controlled by the client.
  - The Proxy design pattern's goal is generally to preserve exactly (or as closely as possible) the API that is being used while offering certain internal enhancements.