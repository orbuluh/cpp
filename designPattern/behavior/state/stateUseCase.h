#pragma once

#include "state.h"
#include <iostream>

class ContextStateMachine {
public:
    enum class Input {
        PressB,
        PressDown,
        ReleaseDown,
    };
    static constexpr const char* InputStr[] =
        { "PressB", "PressDown", "ReleaseDown" };
private:
    using BaseStateT = State<ContextStateMachine, Input>;
    class StandingState : public BaseStateT {
    public:
        ~StandingState() override = default;
        BaseStateT* handleInput(ContextStateMachine* context, Input input) override;
        void enter() override {
            std::cout << "State::Standing" << '\n';
        }
    };
    class JumpingState : public BaseStateT {
    public:
        ~JumpingState() override = default;
        BaseStateT* handleInput(ContextStateMachine* context, Input input) override;
        void enter() override {
            std::cout << "State::Jumping" << '\n';
        }
    };
    class DivingState : public BaseStateT {
    public:
        ~DivingState() override = default;
        BaseStateT* handleInput(ContextStateMachine* context, Input input) override;
        void enter() override {
            std::cout << "State::Diving" << '\n';
        }
    };
public:
    ContextStateMachine() : state_(new StandingState()) {}
    virtual ~ContextStateMachine() = default;
    void handleInput(Input input) {
        auto newState = state_->handleInput(this, input);
        if (newState) {
            state_.reset(newState);
            state_->enter();
        }
    }
protected:
    std::unique_ptr<BaseStateT> state_;
};


inline
ContextStateMachine::BaseStateT* ContextStateMachine::StandingState::handleInput(ContextStateMachine* context, Input input) {
    std::cout << "receive input: " << InputStr[static_cast<int>(input)] << '\n';
    if (input == Input::PressB) {
        return new JumpingState{};
    } else if (input == Input::PressDown) {
        return new DivingState{};
    } else if (input == Input::ReleaseDown) {
        return BaseStateT::NoChangeState;
    }
    return BaseStateT::NoChangeState;
}

inline
ContextStateMachine::BaseStateT* ContextStateMachine::JumpingState::handleInput(ContextStateMachine* context, Input input) {
    std::cout << "receive input: " << InputStr[static_cast<int>(input)] << '\n';
    if (input == Input::PressB) {
        return BaseStateT::NoChangeState;
    } else if (input == Input::PressDown) {
        return new StandingState{};
    } else if (input == Input::ReleaseDown) {
        return BaseStateT::NoChangeState;
    }
    return BaseStateT::NoChangeState;
}

inline
ContextStateMachine::BaseStateT* ContextStateMachine::DivingState::handleInput(ContextStateMachine* context, Input input) {
    std::cout << "receive input: " << InputStr[static_cast<int>(input)] << '\n';
    if (input == Input::PressB) {
        return new JumpingState{};
    } else if (input == Input::PressDown) {
        return BaseStateT::NoChangeState;
    } else if (input == Input::ReleaseDown) {
        return new StandingState{};
    }
    return BaseStateT::NoChangeState;
}

namespace state {
void demo();
} // namespace state