# Quick fact
- Visitor is a behavioral design pattern that lets you separate algorithms from the objects on which they operate.
- Whenever people speak of visitors, the word dispatch is brought up. Put simply, “dispatch” is a problem of figuring out which function to call—specifically, how many pieces of information are required in order to make the call.

# Applicability
- Use the Visitor when you need to perform an operation on all elements of a complex object structure (for example, an object tree).
- Use the Visitor to clean up the business logic of auxiliary behaviors.
- Use the pattern when a behavior makes sense only in some classes of a class hierarchy, but not in others.
- You perform operations of a class of objects and want to avoid run-time type testing. This is usually the case when you traverse a hierarchy of disparate objects having different properties.
