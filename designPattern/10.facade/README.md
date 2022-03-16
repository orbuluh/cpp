# Quick fact
- Façade is a simplified representation of what is a rather complicated set-up behind the scenes.
    - A way of putting a simple interface in front of one or more complicated subsystems.
    - Provides a simplified interface to a library, a framework, or any other complex set of classes.
- A facade might provide limited functionality in comparison to working with the subsystem directly. However, it includes only those features that clients really care about.
- Having a facade is handy when you need to integrate your app with a sophisticated library that has dozens of features, but you just need a tiny bit of its functionality.
- This structure also helps you to minimize the effort of upgrading to future versions of the framework or replacing it with another one.

# Applicability
- Use the Facade pattern when you need to have a limited but straightforward interface to a complex subsystem.
- Use the Facade when you want to structure a subsystem into layers.
    - You can reduce coupling between multiple subsystems by requiring them to communicate only through facades.


# Implementation tips
- You’re on the right track if this interface makes the client code independent from many of the subsystem’s classes.
- The facade should redirect the calls from the client code to appropriate objects of the subsystem.
- make all the client code communicate with the subsystem only via the facade.

# Comparison
- Facade defines a simplified interface to a subsystem of objects, but it doesn’t introduce any new functionality. The subsystem itself is unaware of the facade.
- Mediator centralizes communication between components of the system. The components only know about the mediator object and don’t communicate directly.
- Facade is similar to Proxy in that both buffer a complex entity and initialize it on its own. Unlike Facade, Proxy has the same interface as its service object, which makes them interchangeable.