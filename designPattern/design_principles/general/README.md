# Reference
* Mainly from [Dive Into Design Patterns](https://refactoring.guru/design-patterns/book)

# Features of Good Design
## Code reuse
- making existing code work in a new context usually takes extra effort. Tight coupling between components, dependencies on concrete classes instead of interfaces, hardcoded operations—all of this reduces flexibility of the code and makes it harder to reuse it.
- Using design patterns is one way to increase flexibility of software components and make them easier to reuse. However, this sometimes comes at the price of making the components more complicated.
- Three level of reuse from Erich Gamma:
  - Lowest level, you reuse classes.
    - class libraries, containers etc.
  - Highest level, frameworks.
    - Frameworks identify the key abstractions for solving a problem, represent the key abstractions by classes and define relationships between them.
    - You hook into frameworks by subclassing somewhere. Framework lets you define your custom behavior, and it will call you when it's your turn to do something.
  - Middle level, design patterns.
    - They're really a description about how a couple of classes can relate to and interact with each other.
    - Design patterns offer reuse in a way that is less risky than frameworks. Patterns let you reuse design ideas and concepts independently of concrete code.
- The level of reuse increases when you move from classes to patterns and finally frameworks.

## Extensibility
- Change is the only constant thing in a programmer’s life. Each software developer has dozens of similar stories. There are several reasons why this happens.
  - First, we understand the problem better once we start to solve it. Often by the time you finish the first version of an app, you’re ready to rewrite it from scratch because now you understand many aspects of the problem much better. You have also grown professionally, and your own code now looks like crap.
  - Something beyond your control has changed. This is why so many dev teams pivot from their original ideas into something new.
  - The third reason is that the goalposts move. Your client was delighted with the current version of the application, but now sees eleven “little” changes he’d like so it can do other things he never mentioned in the original planning sessions. These aren’t frivolous changes: your excellent first version has shown him that even more is possible.
- That’s why all seasoned developers try to provide for possible future changes when designing an application’s architecture.

# Design principles
## Encapsulate What Varies
- Identify the aspects of your application that vary and separate them from what stays the same.
- The main goal of this principle is to minimize the effect caused by changes.
- Through this, you can isolate the parts of the program that vary in independent modules, protecting the rest of the code from adverse effects.
- As a result, you spend less time getting the program back into working shape, implementing and testing the changes.
- The less time you spend making changes, the more time you have for implementing features.

## Program to an Interface, not an Implementation
- Depend on abstractions, not on concrete classes.
- You can tell that the design is flexible enough if you can easily extend it without breaking any existing code.
