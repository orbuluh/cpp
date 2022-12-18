Mostly from reference [DIDP](https://refactoring.guru/design-patterns/book) - well written!
# Quick fact
- The Command design pattern is simple: what it basically suggests is that objects can communicate with one another using special objects that encapsulate instructions, rather than specifying those same instructions as arguments to a method.
- It turns a request into a stand-alone object that contains all information about the request. 
    - This transformation lets you parameterize methods with different requests, delay or queue a request’s execution, and support undoable operations.
- Commands are used a lot in UI systems to encapsulate typical actions (e.g., Copy or Paste) and then allow a single command to be invoked by several different means.

# Pros
- You can decouple classes that invoke operations from classes that perform these operations.
- You can introduce new commands into the app without breaking existing client code.
- You can implement undo/redo.
- You can implement deferred execution of operations.
- You can assemble a set of simple commands into a complex one.

# Cons
- The code may become more complicated since you’re introducing a whole new layer between senders and receivers.

# Components
- Sender/invoker triggers that command instead of sending the request directly to the receiver.
- The Command interface usually declares just a single method for executing the command.
    - Concrete Commands implement various kinds of requests.
    - A concrete command isn’t supposed to perform the work on its own, but rather to pass the call to one of the business logic objects.
- The Receiver (of command) contains some business logic and does the actual work.
    - Most commands only handle the details of how a request is passed to the receiver.
# Analogy:
- waiter (sender/invoker) takes order from client and write to a paper (command).
- waiter stick paper to the kitchen, chef (receiver, who owns business logic) cooks according to the paper, and put the food to the tray.
- waiter check the tray and return the food to client.

# Applicability
- Use the Command pattern when you want to parametrize objects with operations.
    - The Command pattern can turn a specific method call into a stand-alone object.
    - This change opens up a lot of interesting uses: you can pass commands as method arguments, store them inside other objects, switch linked commands at run-time, etc.
- Use the Command pattern when you want to queue operations, schedule their execution, or execute them remotely.
    - a command can be serialized, which means converting it to a string that can be easily written to a file or a database. Later, the string can be restored as the initial command object. Thus, you can delay and schedule command execution.
    - Also you can queue, log or send commands over the network.
- Use the Command pattern when you want to implement reversible operations.
    - Although there are many ways to implement undo/redo, the Command pattern is perhaps the most popular of all.
    - To be able to revert operations, you need to implement the history of performed operations. The command history is a stack that contains all executed command objects along with related backups of the application’s state.
    - sometimes you can resort to an alternative implementation: instead of restoring the past state, the command performs the inverse operation. (it may turn out to be hard or even impossible to implement.)