# Quick fact
- Builder is a creational design pattern that lets you construct complex objects step by step. The pattern allows you to produce different types and representations of an object using the same construction code.
- The Builder pattern is concerned with the creation of complicated objects, that is, objects that cannot be built up in a single-line constructor call. These types of objects may themselves be composed of other objects and might involve less-than-obvious logic, necessitating a separate component specifically dedicated to object construction.
- A single builder interface can expose multiple subbuilders. Through clever use of inheritance and fluent interfaces, one can jump from one builder to another with ease.
- You can go further and extract a series of calls to the builder steps you use to construct a product into a separate class called **director**. The director class defines the order in which to execute the building steps, while the builder provides the implementation for those steps. The director class might be a good place to put various construction routines so you can reuse them across your program.

# Applicability
- Use the Builder pattern to get rid of a “telescopic constructor”
  - Say you have a constructor with ten optional parameters. Calling such a beast is very inconvenient; therefore, you overload the constructor and create several shorter versions with fewer parameters. These constructors still refer to the main one, passing some default values into any omitted parameters.
- Use the Builder pattern when you want your code to be able to create different representations of some product
- Use the Builder to construct Composite trees or other complex objects.

# Comparison
- Many designs start by using Factory Method (less complicated and more customizable via subclasses) and evolve toward Abstract Factory, Prototype, or Builder.
