# Quick fact
- Prototype is a creational design pattern that lets you copy existing objects without making your code dependent on their classes.
- The Prototype pattern delegates the cloning process to the actual objects that are being cloned.
- The pattern declares a common interface for all objects that support cloning. This interface lets you clone an object without coupling your code to the class of that object. Usually, such an interface contains just a single clone method.
- An object that supports cloning is called a prototype.
- Here’s how it works: you create a set of objects, configured in various ways. When you need an object like the one you’ve configured, you just clone a prototype instead of constructing a new object from scratch.
- Optionally, create a centralized prototype registry to store a catalog of frequently used prototypes. This method should search for a prototype based on search criteria that the client code passes to the method. Then replace the direct calls to the subclasses’ constructors with calls to the factory method of the prototype registry.

# Caveat
- The Prototype pattern is all about object copying. And, of course, we do not have a uniform way of actually copying an object, but there are options, and we’ll choose some of them. The challenge of the Prototype pattern is really the copying part; everything else is easy.
- Method 1: write customized copy constructor/assignment to make sure deep copy
- Method 2: use libs that support de/serialization - `boost::serialization` for example.

# Applicability
- The Prototype pattern provides the client code with a general interface for working with all objects that support cloning. This interface makes the client code independent from the concrete classes of objects that it clones.
- Use the pattern when you want to reduce the number of subclasses that only differ in the way they initialize their respective objects. Instead of instantiating a subclass that matches some configuration, the client can simply look for an appropriate prototype and clone it.

# Comparison
- Designs that make heavy use of Composite and Decorator can often benefit from using Prototype. Applying the pattern lets you clone complex structures instead of re-constructing them from scratch.