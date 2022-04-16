# Quick fact
- The Flyweight pattern is fundamentally a space-saving technique by sharing common parts of state between multiple objects instead of keeping all of the data in each object.
- The Flyweight pattern is merely an optimization. Before applying it, make sure your program does have the RAM consumption problem related to having a massive number of similar objects in memory at the same time. Make sure that this problem can’t be solved in any other meaningful way. As The code becomes much more complicated when applying the pattern, new team members will always be wondering why the state of an entity was separated in such a way.
- string_view is a flyweight, small string optimization should be a flyweight as well.

# Intrinsic / extrinsic state and flyweight
- This constant data of an object is usually called the **intrinsic state**.
- The rest of the object’s state, often altered “from the outside” by other objects, is called the **extrinsic state**.
- The Flyweight pattern suggests that
  - Only the intrinsic state stays within the object, stop storing the extrinsic state inside the object.
  - The object that **only stores the intrinsic state is called a flyweight.**
  - Extrinsic state is passed to specific methods which rely on it for different context.
  - And the unchanged intrinsic state can be reused in different contexts.
- As a result, you’d need fewer of these objects since they only differ in the intrinsic state, which has much fewer variations than the extrinsic.
- (The time to apply flyweight basically assume that most memory-consuming fields have been moved to just a few flyweight objects.)
- Because flyweight object is constant (so it can be used in different contexts) - it should initialize its state just once, via constructor parameters. It shouldn’t expose any setters or public fields to other objects.

# The "Context" class in flyweight
- Context class contains the extrinsic state.
- When a context is paired with one of the flyweight objects, it represents the full state of the original object.
- The client must store or calculate values of the extrinsic state (context) to be able to call methods of flyweight objects.

# Flyweight factory
- you can create a factory method that manages a pool of existing flyweight objects.
- The method accepts the intrinsic state of the desired flyweight from a client, looks for an existing flyweight object matching this state, and returns it if it was found. If not, it creates a new flyweight and adds it to the pool.
- The Flyweight Factory manages a pool of existing flyweights. With the factory, clients don’t create flyweights directly.

