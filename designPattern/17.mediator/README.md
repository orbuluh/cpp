# Quick fact
- Mediator is a mechanism for facilitating communication between the components.
    - there are situations when you don’t want objects to be necessarily aware of each other’s presence.
    - Or, perhaps you do want them to be aware of one another, but you still don’t want them to communicate through pointers or references
    - Overall, it lets you reduce chaotic dependencies between objects.
    - The pattern restricts direct communications between the objects and forces them to collaborate only via a mediator object.
![mediator concept](../../src/images/mediator.png)
- The mediator itself needs to be accessible to every component taking part, maybe a reference that gets injected into every component.
- The Mediator pattern suggests that you should cease all direct communication between the components which you want to make independent of each other. Instead, these components must collaborate indirectly, by calling a special mediator object that redirects the calls to appropriate components.
- As a result, the components depend only on a single mediator class instead of being coupled to dozens of their colleagues.