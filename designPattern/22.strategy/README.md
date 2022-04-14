# Quick fact
- Strategy is a behavioral design pattern that lets you define a family of algorithms, put each of them into a separate class, and make their objects interchangeable.
- The Strategy pattern suggests that you take a class that does something specific in a lot of different ways and extract all of these algorithms into separate classes called strategies.
- The original class, called context, must have a field for storing a reference to one of the strategies. The context delegates the work to a linked strategy object instead of executing it on its own.
- The context isn’t responsible for selecting an appropriate algorithm for the job. Instead, the client passes the desired strategy to the context. In fact, the context doesn’t know much about strategies. It works with all strategies through the same generic interface, which only exposes a single method for triggering the algorithm encapsulated within the selected strategy.

# Applicability
- Use the Strategy when you have a lot of similar classes that only differ in the way they execute some behavior.
- Use the pattern to isolate the business logic of a class from the implementation details of algorithms that may not be as important in the context of that logic.
  - The Strategy pattern lets you isolate the code, internal data, and dependencies of various algorithms from the rest of the code.
  - Various clients get a simple interface to execute the algorithms and switch them at runtime.
- Use the pattern when your class has a massive conditional operator that switches between different variants of the same algorithm.
  - The Strategy pattern lets you do away with such a conditional by extracting all algorithms into separate classes, all of which implement the same interface. The original object delegates execution to one of these objects, instead of implementing all variants of the algorithm.

# Comparison
- Bridge, State, Strategy (and to some degree Adapter) have very similar structure that are based on composition, which is delegating work to other objects. However, they all solve different problems.
- Command and Strategy may look similar because you can use both to parameterize an object with some action. However, they have very different intents.
- You can use Command to convert any operation into an object. The operation’s parameters become fields of that object. The conversion lets you defer execution of the operation, queue it, store the history of commands, send commands to remote services, etc.
- On the other hand, Strategy usually describes different ways of doing the same thing, letting you swap these algorithms within a single context class.
- Decorator lets you change the skin of an object, while Strategy lets you change the guts
- Template Method is based on inheritance: it lets you alter parts of an algorithm by extending those parts in subclasses.
- Strategy is based on composition: you can alter parts of the object’s behavior by supplying it with different strategies that correspond to that behavior.
- Template Method works at the class level, so it’s static. Strategy works on the object level, letting you switch behaviors at runtime.
- State can be considered as an extension of Strategy. Both patterns are based on composition: they change the behavior of the context by delegating some work to helper objects. Strategy makes these objects completely independent and unaware of each other. However, State doesn’t restrict dependencies between concrete states, letting them alter the state of the context at will.