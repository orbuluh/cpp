# Quick fact
- The Visitor pattern is a pattern that **separates the algorithm from the object structure, which is the data for this algorithm**
  - Using the Visitor pattern, we can add a new operation to the class hierarchy without modifying the classes themselves.
- Whenever people speak of visitors, the word **dispatch** is brought up. Put simply, “dispatch” is a problem of figuring out **which function to call—specifically**, **how many pieces of information are required in order to make the call**.
- basic structure of the Visitor pattern is quite complex and involves many coordinated classes that must work together to form the pattern.
  - On the other hand, even the description of the Visitor pattern is complex—there are several very different ways to describe the same pattern.

# The use of the Visitor pattern follows the open/closed principle of the software design
- the Visitor pattern allows us to **add functionality to a class or the entire class hierarchy without having to modify the class.**
  - This feature can be particularly useful when **dealing with public APIs—the users of the API can extend it with additional operations without having to modify the source code.**
- a class (or another unit of code, such as a module) should be **closed for modifications**;
  - once the class presents an interface to its clients, the clients come to depend on this interface and the functionality it provides.
  - This interface should remain stable; it should not be necessary to modify the classes in order to maintain the software and continue its development.
- At the same time, a class should be **open for extensions—new functionality** can be added to satisfy new requirements.


# Applicability
- Use the Visitor when you need to perform an operation on all elements of a complex object structure (for example, an object tree).
- Use the Visitor to **clean up the business logic of auxiliary behaviors**.
- Use the pattern when **a behavior makes sense only in some classes of a class hierarchy**, but not in others.
- You perform operations of a class of objects and want to avoid run-time type testing. This is usually the case **when you traverse a hierarchy of disparate objects having different properties**.
