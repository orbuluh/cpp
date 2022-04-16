# Quick fact
- Chain of Responsibility (CoR) is a behavioral design pattern that lets you pass requests along a chain of handlers.
    - the CoR relies on transforming particular behaviors into stand-alone objects called handlers. The pattern suggests that you link these handlers into a chain.
    - Upon receiving a request, each handler decides either to process the request or to pass it to the next handler in the chain.
    - The request travels along the chain until all handlers have had a chance to process it.
    - a handler can decide not to pass the request further down the chain and effectively stop any further processing.
- It’s crucial that all handler classes implement the same interface.
    - Each concrete handler should only care about the following one having the execute method.
    - This way you can compose chains at runtime, using various handlers without coupling your code to their concrete classes.
- The client may either assemble chains on its own or receive pre-built chains from other objects.
    - In the latter case, you must implement some factory classes to build chains according to the configuration or environment settings.
- The simplest implementation of CoR is one where you simply make a pointer chain and, in theory, you could replace it with just an ordinary vector or, perhaps, a list if you wanted fast removal as well.
- A more sophisticated Broker Chain implementation that also leverages the Mediator and Observer patterns allows us to process queries on an event (signal), letting each subscriber perform modifications of the originally passed object (it’s a single reference that goes through the entire chain) before the final values are returned to the client.

# Applicability
- when your program is expected to process different kinds of requests in various ways, but the exact types of requests and their sequences are unknown beforehand.
- when it’s essential to execute several handlers in a particular order.
    - Since you can link the handlers in the chain in any order, all requests will get through the chain exactly as you planned.
- when the set of handlers and their order are supposed to change at runtime.

# Pros
- You can control the order of request handling.
- Single Responsibility Principle. You can decouple classes that invoke operations from classes that perform operations.
- You can introduce new handlers into the app without breaking the existing client code.

# Comparison
- CoR passes a request sequentially along a dynamic chain of potential receivers until one of them handles it.
- Command establishes unidirectional connections between senders and receivers.
    - Handlers in CoR can be implemented as Commands. In this case, you can execute a lot of different operations over the same context object, represented by a request.
- Mediator eliminates direct connections between senders and receivers, forcing them to communicate indirectly via a mediator object.
- Observer lets receivers dynamically subscribe to and unsubscribe from receiving requests.
- CoR and Decorator have very similar class structures. Both patterns rely on recursive composition to pass the execution through a series of objects.
- Various Decorators can extend the object’s behavior while keeping it consistent with the base interface. In addition, decorators aren’t allowed to break the flow of the request. CoR handlers can execute arbitrary operations independently of each other. They can also stop passing the request further at any point.