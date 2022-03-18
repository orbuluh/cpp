# Quick fact
- Mediator is a mechanism for facilitating communication between the components.
    - there are situations when you don’t want objects to be necessarily aware of each other’s presence.
    - Or, perhaps you do want them to be aware of one another, but you still don’t want them to communicate through pointers or references
- The mediator itself needs to be accessible to every component taking part, maybe a reference that gets injected into every component.