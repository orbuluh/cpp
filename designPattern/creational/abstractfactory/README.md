# Quick fact
- **Abstract Factory** is a creational design pattern that lets you produce families of related objects without specifying their concrete classes.
- The first thing the Abstract Factory pattern suggests is to explicitly declare interfaces for each distinct product of the product family (e.g., chair, sofa or coffee table). Then you can make all variants of products follow those interfaces. The next move is to declare the Abstract Factory—an interface with a list of creation methods for all products that are part of the product family. Now, how about the product variants? For each variant of a product family, we create a separate factory class based on the AbstractFactory interface.

# Applicability
- Use the Abstract Factory when your code needs to work with various families of related products, but you don’t want it to depend on the concrete classes of those products—they might be unknown beforehand or you simply want to allow for future extensibility.