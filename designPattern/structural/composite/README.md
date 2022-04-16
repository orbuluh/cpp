# Quick fact
- Composite is a structural design pattern that lets you compose objects into tree structures and then work with these structures as if they were individual objects.
- The Composite pattern provides you with two basic element types that share a common interface: simple leaves and complex containers.
  - A container can be composed of both leaves and other containers.
  - This lets you construct a nested recursive object structure that resembles a tree.
- All elements defined by the Composite pattern share a common interface.
  - Using this interface, the client doesn’t have to worry about the concrete class of the objects it works with.
  - The Component interface describes operations that are common to both simple and complex elements of the tree.
- The Container (aka composite) is an element that has sub-elements: leaves or other containers.
  - A container doesn’t know the concrete classes of its children. It works with all sub-elements only via the component interface.
  - Upon receiving a request, a container delegates the work to its sub-elements, processes intermediate results and then returns the final result to the client.
- The Client works with all elements through the component interface. As a result, the client can work in the same way with both simple or complex elements of the tree.

# Applicability
- Using the Composite pattern makes sense only when the core model of your app can be represented as a tree.
- Or, use the Composite pattern when you have to implement a tree-like object structure.

# Comparison
- Chain of Responsibility is often used in conjunction with Composite. In this case, when a leaf component gets a request, it may pass it through the chain of all of the parent components down to the root of the object tree.
- A Decorator is like a Composite but only has one child component. There’s another significant difference: Decorator adds additional responsibilities to the wrapped object, while Composite just “sums up” its children’s results
- Designs that make heavy use of Composite and Decorator can often benefit from using Prototype. Applying the pattern lets you clone complex structures instead of re-constructing them from scratch.