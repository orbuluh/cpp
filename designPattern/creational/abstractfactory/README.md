# Quick fact
- **Abstract Factory** is a creational design pattern that lets you produce families of related objects without specifying their concrete classes.
- The first thing the Abstract Factory pattern suggests is to explicitly declare interfaces for each distinct product of the product family (e.g., chair, sofa or coffee table). Then you can make all variants of products follow those interfaces. The next move is to declare the Abstract Factory—an interface with a list of creation methods for all products that are part of the product family. Now, how about the product variants? For each variant of a product family, we create a separate factory class based on the AbstractFactory interface.

# Applicability
- Use the Abstract Factory when your code needs to work with various families of related products, but you don’t want it to depend on the concrete classes of those products—they might be unknown beforehand or you simply want to allow for future extensibility.

# [Dependency injection](https://www.codeproject.com/Articles/778264/Dependency-Injection-of-an-Abstract-Factory)
- Usually, if a class of type A uses a class of type B (flow of functional dependency), B needs to be compiled before A. But when we introduce an interface for class B, put it into the component of A and delegate the creation of the concrete implementation of B to an external Factory Container, A could be compiled before B.
- Say A depends on B, through injecting/passing a "B factory" to A, you can manipulate what B actually used in A. (E.g. you could have passed a MockB or ActualB ... etc)

# Comparison
- Many designs start by using Factory Method (less complicated and more customizable via subclasses) and evolve toward Abstract Factory, Prototype, or Builder (more flexible, but more complicated).
- Builder focuses on constructing complex objects step by step. Abstract Factory specializes in creating families of related objects. Abstract Factory returns the product immediately, whereas Builder lets you run some additional construction steps before fetching the product.
- Abstract Factory classes are often based on a set of Factory Methods, but you can also use Prototype to compose the methods on these classes.