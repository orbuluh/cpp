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
- Other objects must communicate with mementos using** a limited interface** which may allow fetching the snapshot’s metadata (creation time, the name of the performed operation, etc.), but **not the original object’s state contained in the snapshot**.


stores the state of the system and returns it as a dedicated, read-only object with no behavior of its own. This “token,” if you will, can be used only for feeding it back into the system to restore it to the state it represents.
The Memento class is immutable. Imagine if you could, in fact, change the balance: you could roll back the account to a state it was never in!
