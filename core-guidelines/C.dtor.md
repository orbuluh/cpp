# C.dtor: Destructors
- "Does this class need a destructor?" is a surprisingly insightful design question.
  - For most classes the answer is "no" either because the class holds no resources or because destruction is handled by the rule of zero; that is, its members can take care of themselves as concerns destruction.
  - If the answer is "yes", much of the design of the class follows (see the rule of five).

## C.30: Define a destructor if a class needs an explicit action at object destruction
- Only define a non-default destructor if a class needs to execute code that is not already part of its members' destructors.
- There are two general categories of classes that need a user-defined destructor:
  - A class with a resource that is not already represented as a class with a destructor, e.g., a vector or a transaction class.
  - A class that exists primarily to execute an action upon destruction, such as a tracer or final_action.

```cpp
template<typename A>
struct final_action {   // slightly simplified
    A act;
    final_action(A a) : act{a} {}
    ~final_action() { act(); }
};

template<typename A>
final_action<A> finally(A act)   // deduce action type
{
    return final_action<A>{act};
}

void test()
{
    auto act = finally([] { cout << "Exit test\n"; });  // establish exit action
    // ...
    if (something) return;   // act done here
    // ...
} // act done here
```
- If the default destructor is needed, but its generation has been suppressed (e.g., by defining a move constructor), use `=default`