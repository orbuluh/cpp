#include "../cookbook.h"

// back-end
#include <boost/msm/back/state_machine.hpp>
// front-end
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>
// mpl
#include <boost/mpl/vector.hpp>

#include <iostream>
#include <vector>

namespace boost_msm {

namespace msm = boost::msm;
namespace mpl = boost::mpl;

// events
struct Init { std::string name = "Init"; };
struct PressB { std::string name = "PressB"; };
struct PressDown { std::string name = "PressDown"; };
struct ReleaseDown { std::string name = "ReleaseDown"; };

// front-end: define the FSM structure
struct PlayerFormFSM : msm::front::state_machine_def<PlayerFormFSM>
{
    template <class Event, class FSM>
    void on_entry(Event const& evt, FSM& fsm) {
        std::cout << "enter: " << evt.name << std::endl;
    }

    template <class Event, class FSM>
    void on_exit(Event const& evt, FSM& fsm) {
        std::cout << "leaving: " << evt.name << std::endl;
    }

    // The list of FSM states
    struct SimpleNameState : public msm::front::state<> {
        SimpleNameState(std::string_view nameSv) : name(nameSv) {}

        template <class Event,class FSM>
        void on_entry(Event const&,FSM& ) {std::cout << "entering: " << name << std::endl;}

        template <class Event,class FSM>
        void on_exit(Event const&,FSM& ) {std::cout << "leaving: " << name << std::endl;}

        std::string name = "TBD";
    };

    struct StandingState : public SimpleNameState {
        StandingState() : SimpleNameState("Standing") {};
    };
    struct JumpingState : public SimpleNameState {
        JumpingState() : SimpleNameState("Jumping") {};
    };
    struct DivingState :public SimpleNameState {
        DivingState() : SimpleNameState("Diving") {};
    };

    // the initial state of the player SM. Must be defined
    using initial_state = StandingState;
    // the initial event, optionally defiend
    using initial_event = Init;

    // guard conditions
    struct TransitionGuard { // didn't actually used below
        template <typename EVT, typename FSM, typename SourceState, typename TargetState>
        bool operator()(EVT const&, FSM& fsm, SourceState&, TargetState&) {
            // if (fsm.some_attribute == ...)
            // something like this as a guard
            return true;
        }
    };

    // Replaces the default no-transition response.
    template <class FSM,class Event>
    void no_transition(Event const& e, FSM&, int state)
    {
        std::cout << "no transition from state " << state
            << " on event " << e.name << std::endl;
    }

    struct transition_table : mpl::vector<
        // row    takes 5 arguments: start state, event, target state, action and guard.
        // _row   allows omitting action and guard.
        // a_row  (“a” for action) allows defining only the action and omit the guard condition.
        // g_row  (“g” for guard) allows omitting the action behavior and defining only the guard.
        _row<StandingState, PressB,      JumpingState>,
        _row<StandingState, PressDown,   DivingState>,
        _row<StandingState, ReleaseDown, StandingState>,
        _row<JumpingState,  PressB,      JumpingState>,
        _row<JumpingState,  PressDown,   StandingState>,
        _row<JumpingState,  ReleaseDown, JumpingState>,
        _row<DivingState,   PressB,      JumpingState>,
        _row<DivingState,   PressDown,   DivingState>,
        _row<DivingState,   ReleaseDown, StandingState>
    > {};
};

// Pick a back-end
using PlayerForm = msm::back::state_machine<PlayerFormFSM>;

void demo() {
    PlayerForm p;
    // needed to start the highest-level SM. This will call on_entry and mark the start of the SM
    p.start();
    p.process_event(PressB{});
    p.process_event(PressDown{});
    p.process_event(PressB{});
    p.process_event(ReleaseDown{});
    p.process_event(PressDown{});
}
} // namespace boost_msm