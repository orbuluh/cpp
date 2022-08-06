# [Code](../code/double_dispatch.h)

# Double dispatch is the backbone of visitor pattern
- Using the Visitor pattern, we can add a new operation to the class hierarchy without modifying the classes themselves.
- if we want to add an operation to all classes in a hierarchy, that is equivalent to adding a virtual function, so we have one factor controlling the final disposition of each call, the object type.
- But, if we can effectively add virtual functions, we can add more than one
  - one for each operation we need to support.
- The type of the operation is the second factor controlling the dispatch, similar to the argument to the function in the [basic form](../code/double_dispatch.h)
- Thus, the operation-adding visitor is able to provide double dispatch.
- Alternatively, if we had a way to implement double dispatch, we could do what the Visitor pattern does
  - effectively add a virtual function for each operation we want to support.