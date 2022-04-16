#pragma once
#include <memory>

template <typename ContextT, typename InputT>
class State {
public:
    using StateT = State<ContextT, InputT>;
    virtual ~State() {}
    virtual StateT* handleInput(ContextT* context, InputT input) { return NoChangeState; }
    virtual void enter() {};
    StateT* NoChangeState = nullptr;
};

/*********** ORIGINAL ATTEMPT ***********
// You can't make context also template ...
// Context depends on State and State also depend on Context...

template <typename InputT, typename StateT>
class Context {
    using ContextT = Context<InputT, StateT>;
    using BaseStateT = StateT::BaseStateT;
public:
    Context(BaseStateT* initState) {
        state_.reset(initState);
    }
    virtual ~Context() = default;
    virtual void handleInput(InputT input) {
        auto newState = state_->handleInput(static_cast<ConTextT>(this), input);
        if (newState) {
            state_.reset(newState);
            state_->enter();
        }
    }
protected:
    std::unique_ptr<BaseStateT> state_;
};
***********************************************/