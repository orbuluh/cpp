# Why can I access private variables from different instances of a class

> From [stackoverflow discussions](https://stackoverflow.com/questions/4117002/why-can-i-access-private-variables-in-the-copy-constructor)


The overarching concept here is that it's **the programmer(s)** designing, writing and maintaining a class who **is(are) expected to understand the OO encapsulation desired and empowered to coordinate its implementation.**

So, if you're writing `class X`, you're encoding not just how an individual `X x` object can be used by code with access to it, but also how:

- **derived classes are able to interact with it** (through optionally-pure virtual functions and/or protected access), and
- distinct `X` objects cooperate to **provide intended behaviours while honouring the post-conditions and invariants from your design**.


Note that it's not just the copy constructor that can use private members of other instances, a great many operations can involve two or more instances of your class: 

- if you're comparing, adding/multiplying/dividing, copy-constructing, cloning, assigning etc.

Supposed you need to do all these operations without direct access to the other instances's private member, it would be a non-negligible runtime overhead to check if `this == other` each time you access `other.x` which you would have to if the access modifiers worked on object level.

- It's often the case that you either simply **must have access to private and/or protected data in the other object, or want it to allow a simpler, faster or generally better function implementation.**

So overall, (it's by designed) such that **the access modifiers work on class level, and not on object level.**

- **That is, two objects of the same class can access each others private data.**
- It's also kind of semantically logical if you think of it in terms of scoping: "How big part of the code do I need to keep in mind when modifying a private variable?" – You need to keep the code of the whole class in mind, and **this is orthogonal to which objects exist in runtime.**