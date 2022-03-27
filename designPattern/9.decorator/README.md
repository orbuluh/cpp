# Quick fact
- Decorator is a structural design pattern that lets you attach new behaviors to objects by placing these objects inside special wrapper objects that contain the behaviors.
- A decorator gives a class additional functionality while adhering to the open-close principle.
- Use the Decorator pattern when you need to be able to assign extra behaviors to objects at runtime without breaking the code that uses these objects.
- The Decorator lets you structure your business logic into layers, create a decorator for each layer and compose objects with various combinations of this logic at runtime.
- Use the pattern when it’s awkward or not possible to extend an object’s behavior using inheritance.
- Make sure your business domain can be represented as a primary component with multiple optional layers over it.

# Different types of decorator
- Dynamic decorators can store references (or even store the entire values, if you want!) of the decorated objects and provide dynamic (runtime) composability, at the expense of not being able to access the underlying objects’ own members.
- Static decorators use mixin inheritance (a technique where the class inheriting from its template parameter) to compose decorators at compile-time.
    - This loses any sort of runtime flexibility (you cannot recompose objects) but gives you access to the underlying object’s members.
    - These objects are also fully initializable through constructor forwarding.
- Functional decorators can wrap either blocks of code or particular functions to allow composition of behaviors.

# Comparisons
- Adapter provides a different interface to the wrapped object, Proxy provides it with the same interface, and Decorator provides it with an enhanced interface.
- Chain of Responsibility and Decorator have very similar class structures. The CoR handlers can execute arbitrary operations independently of each other. They can also stop passing the request further at any point. On the other hand, various Decorators can extend the object’s behavior while keeping it consistent with the base interface. In addition, decorators aren’t allowed to break the flow of the request.
- Decorator adds additional responsibilities to the wrapped object, while Composite just “sums up” its children’s results.
- Designs that make heavy use of Composite and Decorator can often benefit from using Prototype. Applying the pattern lets you clone complex structures instead of re-constructing them from scratch.
- Decorator lets you change the skin of an object, while Strategy lets you change the guts.
- Proxy usually manages the life cycle of its service object on its own, whereas the composition of Decorators is always controlled by the client.