
[Reference](https://gameprogrammingpatterns.com/state.html)
# Gist of FSM
- In their pure form, that’s the whole banana: states, inputs, and transitions.
- You have a fixed set of states that the machine can be in.
- The machine can only be in one state at a time.
- A sequence of inputs or events is sent to the machine.
- Each state has a set of transitions, each associated with an input and pointing to a state.

# GOF state pattern
- Allow an object to alter its behavior when its internal state changes. The object will appear to change its class.

```cpp
class HeroineState {
public:
    virtual ~HeroineState() {}
    virtual void handleInput(Heroine& heroine, Input input) {}
    virtual void update(Heroine& heroine) {}
};

class Heroine {
public:
    virtual void handleInput(Input input) { state_->handleInput(*this, input); }
    virtual void update() { state_->update(*this); }
    // Other methods...
private:
    HeroineState* state_;
};

class DuckingState : public HeroineState
{
    void handleInput(Heroine& heroine, Input input) override {
        // if (input == ...)
        //     heroine.state_ = ...
    }
    void update(Heroine& heroine) override {
        //...
    }
}
```

# How to reset state?
- static or instantiate
## static:
- If the state object doesn’t have any other fields, then the only data it stores is a pointer to the internal virtual method table so that its methods can be called.
```cpp
class HeroineState {
  public:
    static StandingState standing;
    static DuckingState ducking;
    static JumpingState jumping;
    static DivingState diving;
    // Other code...
};

class DuckingState : public HeroineState
{
    void update(Heroine& heroine) override {
        //...
        heroine.state_ = &standing;
    }
}
```

## Instantiated states
- if we’re allocating a new state, that means we need to free the current one
- the code that’s triggering the change is in a method in the current state. We don’t want to delete this out from under ourselves.
- Work around - change interface
```cpp
class HeroineState {
public:
    virtual ~HeroineState() {}
    // change: handleInput return new state!
    virtual HeroineState* handleInput(Heroine& heroine, Input input) {}
    virtual void update(Heroine& heroine) {}
};

class Heroine {
public:
    virtual void handleInput(Input input) {
        auto newState = state_->handleInput(*this, input);
        if (newState) {
            delete state_;
            state_ = newState;
        }
    }
    virtual void update() { state_->update(*this); }
    // Other methods...
private:
    HeroineState* state_;
};

class DuckingState : public HeroineState
{
    HeroineState* handleInput(Heroine& heroine, Input input) override {
        if (input == PRESS_UP)
            return new StandingState();
        //...
        return nullptr; // Staty in the state
    }
    void update(Heroine& heroine) override {
        //...
    }
}
```
# Enter and Exit Actions
- The goal of the State pattern is to **encapsulate all of the behavior and data for one state in a single class**
- Say we want each state to control its own graphics, we can handle that by giving the state an entry action
```cpp
class StandingState : public HeroineState
{
    HeroineState* handleInput(Heroine& heroine, Input input) override {
        //...
    }
    void update(Heroine& heroine) override {
    }
    void enter(Heroine& heroine) {
        heroine.setGraphics(IMGAGE_STAND);
    }
}
class Heroine {
public:
    virtual void handleInput(Input input) {
        auto newState = state_->handleInput(*this, input);
        if (newState) {
            delete state_;
            state_ = newState;
            state_->enter(*this);// calling enter function once we switch state
        }
    }
//...
};
```
- One particularly nice thing about entry actions is that they run when you enter the state regardless of which state you’re coming from.
- We can, of course, also extend this to support an exit action. This is just a method we call on the state we’re leaving right before we switch to the new state.
- Most real-world state graphs have multiple transitions into the same state. That means we would end up duplicating some code everywhere that transition occurs. Entry actions give us a place to consolidate that.

# FSMs are a good fit for some problems. But their greatest virtue is also their greatest flaw.
- State machines help you untangle hairy code by enforcing a very constrained structure on it. All you’ve got is a fixed set of states, a single current state, and some hardcoded transitions.
- If you try using a state machine for something more complex like game AI, you will slam face-first into the limitations of that model

