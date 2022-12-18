# Reference
* Mainly from [Dive Into Design Patterns](https://refactoring.guru/design-patterns/book)

# Solid principle
- SOLID is a mnemonic for five design principles intended to make software designs more understandable, flexible and maintainable.
- The cost of applying these principles into a program’s architecture might be making it more complicated than it should be.
- Striving for these principles is good, but always try to be pragmatic and don’t take everything written here as dogma.

# Single Responsibility Principle
## A class should have just one reason to change.
- Try to make every class responsible for a single part of the functionality provided by the software, and make that responsibility entirely encapsulated by (you can also say hidden within) the class.
- The main goal of this principle is reducing complexity.
  - You don’t need to invent a sophisticated design for a program that only has about 200 lines of code. Make a dozen methods pretty, and you’ll be fine.
- The real problems emerge when your program constantly grows and changes.
  - At some point classes become so big that you can no longer remember their details.
  - Code navigation slows down to a crawl, and you have to scan through whole classes or even an entire program to find specific things. The number of entities in program overflows your brain stack, and you feel that you’re losing control over the code.
  - There’s more: if a class does too many things, you have to change it every time one of these things changes. While doing that, you’re risking breaking other parts of the class which you didn’t even intend to change.

# Open/Closed Principle
## Classes should be open for extension but closed for modification.
- The main idea of this principle is to keep existing code from breaking when you implement new features.
  - A class is open if you can extend it, produce a subclass and do whatever you want with it
    - add new methods or fields, override base behavior, etc
  - At the same time, the class is closed (you can also say complete) if it’s ready to be used by other classes
    - its interface is clearly defined and won’t be changed in the future.
- If a class is already developed, tested, reviewed, and included in some framework or otherwise used in an app, trying to mess with its code is risky.
  - Instead of changing the code of the class directly, you can create a subclass and override parts of the original class that you want to behave differently. You’ll achieve your goal but also won’t break any existing clients of the original class.


# Liskov Substitution Principle1
## When extending a class, remember that you should be able to pass objects of the subclass in place of objects of the parent class without breaking the client code.
- This means that the subclass should remain compatible with the behavior of the superclass. When overriding a method, extend the base behavior rather than replacing it with something else entirely.
  - This concept is critical when developing libraries and frameworks because your classes are going to be used by other people whose code you can’t directly access and change.
- Unlike other design principles which are wide open for interpretation, the substitution principle has a set of formal requirements for subclasses, and specifically for their methods.

### Requirements
- **Parameter types** in a method of a subclass should **match or be more abstract** than parameter types in the method of the superclass.
- **The return type** in a method of a subclass should **match or be a subtype** of the return type in the method of the superclass.
  - Return type are inverse to requirements for parameter types.
- A method in a subclass shouldn’t throw types of exceptions which the base method isn’t expected to throw.
  - In other words, types of exceptions should match or be subtypes of the ones that the base method is already able to throw.
  - This rule comes from the fact that try-catch blocks in the client code target specific types of exceptions which the base method is likely to throw. Therefore, an unexpected exception might slip through the defensive lines of the client code and crash the entire application.
  - In most modern programming languages, especially statically typed ones. You won’t be able to compile a program that violates these rules.
- A **subclass shouldn’t strengthen pre-conditions**.
  - For example, the base method has a parameter with type int.
  - If a subclass overrides this method and requires that the value of an argument passed to the method should be positive (by throwing an exception if the value is negative), this strengthens the pre-conditions.
  - The client code, which used to work fine when passing negative numbers into the method, now breaks if it starts working with an object of this subclass.
- A **subclass shouldn’t weaken post-conditions**.
  - Say you have a class with a method that works with a database. A method of the class is supposed to always close all opened database connections upon returning a value.
  - You created a subclass and changed it so that database connections remain open so you can reuse them.
  - But the client might not know anything about your intentions. Because it expects the methods to close all the connections, it may simply terminate the program right after calling the method, polluting a system with ghost database connections.
- **Invariants of a superclass must be preserved**.
  - Invariants are conditions in which an object makes sense.
  - The confusing part about invariants is that while they can be defined explicitly in the form of interface contracts or a set of assertions within methods, they could also be implied by certain unit tests and expectations of the client code.
  - The rule on invariants is the easiest to violate because you might misunderstand or not realize all of the invariants of a complex class. Therefore, the safest way to extend a class is to introduce new fields and methods, and not mess with any existing members of the superclass. Of course, that’s not always doable in real life.
- A **subclass shouldn’t change values of private fields of the superclass.**
  - some programming languages let you access private members of a class via reflection mechanisms or don’t have any protection for the private members at all.

# Interface Segregation Principle
## Clients shouldn’t be forced to depend on methods they do not use.
- Try to make your interfaces narrow enough that client classes don’t have to implement behaviors they don’t need.
- According to the interface segregation principle, you should break down “fat” interfaces into more granular and specific ones.Clients should implement only those methods that they really need.
- Otherwise, a change to a “fat” interface would break even clients that don’t use the changed methods.
- Class inheritance lets a class have just one superclass, but it doesn’t limit the number of interfaces that the class can implement at the same time. Hence, there’s no need to cram tons of unrelated methods to a single interface.
- Break it down into several more refined interfaces—you can implement them all in a single class if needed.
- However, some classes may be fine with implementing just one of them.

# Dependency Inversion Principle
## High-level classes shouldn’t depend on low-level classes. Both should depend on abstractions.
## Abstractions shouldn’t depend on details. Details should depend on abstractions.
- Usually when designing software, you can make a distinction between two levels of classes.
  - Low-level classes implement basic operations such as working with a disk, transferring data over a network, connecting to a database, etc.
  - High-level classes contain complex business logic that directs low-level classes to do something.
- Sometimes people design low-level classes first and only then start working on high-level ones.
  - This is very common when you start developing a prototype on a new system, and you’re not even sure what’s possible at the higher level because low-level stuff isn’t yet implemented or clear.
  - With such an approach business logic classes tend to become dependent on primitive low-level classes.
  - The dependency inversion principle suggests changing the direction of this dependency.
    - For starters, you need to describe interfaces for low-level operations that high-level classes rely on, preferably in business terms.
    - Now you can make high-level classes dependent on those interfaces, instead of on concrete low-level classes.
    - Once low-level classes implement these interfaces, they become dependent on the business logic level, reversing the direction of the original dependency
