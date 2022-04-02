# Quick fact
- Memento lets you save and restore the previous state of an object without revealing the details of its implementation.
- The Memento pattern **delegates creating the state snapshots to the actual owner of that state, the originator object.**
  - If not doing this way, you will encounter problems to snapshot the state of originator because you either expose all internal details of classes, making them too fragile, or restrict access to their state, making it impossible to produce snapshots.
    - If you make originator's member to public, but when you needs to refactor the originator, you need to clean up state as well.
    - To make a snapshot, you’d need to collect these values and put them into some kind of container. Therefore the containers would probably end up being objects of one class. The class would have almost no methods, but lots of fields that mirror the editor’s state.
    - To allow other objects to write and read data to and from a snapshot, you’d probably need to make its fields public. That would expose all the fields, private or not.
    - Other classes would become dependent on every little change to the snapshot class, which would otherwise happen within private fields and methods without affecting outer classes.
- The pattern suggests storing the copy of the object’s state in a special object called **memento**.
- The contents of the memento aren’t accessible to any other object except the one that produced it. 
- Other objects must communicate with mementos using **a limited interface** which may allow fetching the snapshot’s metadata (creation time, the name of the performed operation, etc.), but **not the original object’s state contained in the snapshot**.


- Use the Memento pattern when you want to produce snapshots of the object’s state to be able to restore a previous state of the object.
- Use the pattern when direct access to the object’s fields/getters/setters violates its encapsulation.

- You can use Command and Memento together when implementing “undo”. In this case, commands are responsible for performing various operations over a target object, while mementos save the state of that object just before a command gets executed.
- Sometimes Prototype can be a simpler alternative to Memento. This works if the object, the state of which you want to store in the history, is fairly straightforward and doesn’t have links to external resources, or the links are easy to re-establish.
