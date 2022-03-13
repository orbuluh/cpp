# Quick fact
In the real world, state machines are more complicated.
- Sometimes, you want some action to occur when a state is reached.
- At other times, you want transitions to be conditional, that is, you want a transition to occur only if some condition holds.

- state machine is a class that inherits from `state_ machine_def` via CRTP.
- each state can also reside in the state machine, and is expected to inherit from the `state` class.
- the state can also define behaviors that happen when you enter (`on_entry`) or exit (`on_exit`) a particular state.
- You can also define behaviors to be executed on a transition (rather than when you’ve reached a state)
    - they don’t need to inherit from anything; instead, they need to provide operator() with a particular signature
- we can also have guard conditions: these dictate whether or not we can actually use a transition in the first place.
- For defining state machine rules, Boost.MSM uses MPL, each row containing, in turn,
    - The source state
    - The transition
    - The target state
    - An optional action to execute
    - An optional guard condition

# Basic front-end: define the FSM structure
* It provides a transition table made of rows of different names and functionality.
* Actions and guards are defined as methods and referenced through a pointer in the transition.
* This front-end provides a simple interface making easy state machines easy to define, but more complex state machines a bit harder.
...

[BoostDoc ... ](https://www.boost.org/doc/libs/1_78_0/libs/msm/doc/HTML/ch03s02.html)
