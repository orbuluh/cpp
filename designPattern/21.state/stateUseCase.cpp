#include "stateUseCase.h"
#include <iostream>

namespace state {
void demo() {
    ContextStateMachine context;
    using Input = ContextStateMachine::Input;
    for (const auto& input : {Input::PressB, Input::PressDown, Input::ReleaseDown}) {
        context.handleInput(input);
    }
}
} // namespace state