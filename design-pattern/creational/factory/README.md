# Quick fact
- **Factory** Method is a creational design pattern that provides an interface for creating objects in a superclass, but allows subclasses to alter the type of objects that will be created.
- At first glance, this change may look pointless: we just moved the constructor call from one part of the program to another. However, now you can override the factory method in a subclass and change the class of products being created by the method.
- The trouble with the constructor is that its name always matches the type. This means we cannot communicate any extra information in it, unlike in an ordinary function. So what can we do? Well, how about making the constructor protected and then exposing some static functions for creating new objects.
- You avoid tight coupling between the creator and the concrete products.

# Applicability
- Use the Factory Method when you don’t know beforehand the exact types and dependencies of the objects your code should work with.
  - The Factory Method separates product construction code from the code that actually uses the product. Therefore it’s easier to extend the product construction code independently from the rest of the code.
- Use the Factory Method when you want to provide users of your library or framework with a way to extend its internal components.
- Use the Factory Method when you want to save system resources by reusing existing objects instead of rebuilding them each time.
- Factory methods can be considered as an alternative to constructors - mostly when constructors aren't expressive enough

# Comparison
- Abstract Factory classes are often based on a set of Factory Methods, but you can also use Prototype to compose the methods on these classes.
- Prototype isn’t based on inheritance, so it doesn’t have its drawbacks. On the other hand, Prototype requires a complicated initialization of the cloned object. Factory Method is based on inheritance but doesn’t require an initialization step.
