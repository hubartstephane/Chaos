#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	namespace SM
	{

		// ==================================================
		// StateBase
		// ==================================================

		StateBase::StateBase(StateMachine * in_state_machine):
			state_machine(in_state_machine)
		{
			assert(in_state_machine != nullptr);
		}

		StateAction StateBase::OnEnterImpl(StateMachineInstance* sm_instance, StateBase* from_state, Object* extra_data)
		{
			return StateAction::CanLeave;
		}

		StateAction StateBase::TickImpl(StateMachineInstance* sm_instance, float delta_time, Object* extra_data)
		{
			return StateAction::CanLeave; // enable automatic transitions
		}

		void StateBase::OnLeaveImpl(StateMachineInstance* sm_instance, StateBase* to_state, Object* extra_data)
		{

		}

		bool StateBase::OnEventReceivedImpl(StateMachineInstance* sm_instance, TagType event_tag, Object* extra_data)
		{
			return false; // do not catch the event : let the outgoing transition do their job
		}

		// ==================================================
		// State
		// ==================================================

		State::State(StateMachine * in_state_machine):
			StateBase(in_state_machine)
		{
		}

		void State::Tick(StateMachineInstance * sm_instance, float delta_time, Object * extra_data)
		{
			StateAction action = TickImpl(sm_instance, delta_time, extra_data);
			if (action == StateAction::MustStay)
				return;
			CheckOutgoingTransitions(sm_instance, extra_data);
		}

		void State::OnEnter(StateMachineInstance * sm_instance, StateBase * from_state, Object * extra_data)
		{
			StateAction action = OnEnterImpl(sm_instance, from_state, extra_data);
			if (action == StateAction::MustStay)
				return;
			CheckOutgoingTransitions(sm_instance, extra_data);
		}

		void State::OnLeave(StateMachineInstance * sm_instance, StateBase * to_state, Object * extra_data)
		{
			OnLeaveImpl(sm_instance, to_state, extra_data);
		}

		bool State::OnEventReceived(StateMachineInstance * sm_instance, TagType event_tag, Object * extra_data)
		{
			// the USER implementation catch the method
			if (OnEventReceivedImpl(sm_instance, event_tag, extra_data))
				return true;
			// give to the outgoing transitions the opportunities to catch the event
			for (Transition * transition : outgoing_transitions)
			{
				if (transition->triggering_event != 0 && transition->triggering_event == event_tag)
				{
					sm_instance->ChangeState(transition, extra_data);
					return true;
				}
			}
			return false; // not handled
		}

		void State::CheckOutgoingTransitions(StateMachineInstance* sm_instance, Object* extra_data)
		{
			// give the outgoing transition the opportunities to trigger themselves
			for (Transition* transition : outgoing_transitions)
			{
				StateAction action = transition->CheckTransitionConditions(sm_instance, extra_data);
				if (action == StateAction::MustStay)
					continue;
				sm_instance->ChangeState(transition, extra_data);
				return;
			}
		}

		// ==================================================
		// Transition
		// ==================================================

		Transition::Transition(State * in_from_state, State * in_to_state, TagType in_triggering_event):
			StateBase(in_from_state->state_machine),
			from_state(in_from_state),
			to_state(in_to_state),
			triggering_event(in_triggering_event)
		{
			assert(in_from_state != nullptr);
			assert(in_to_state != nullptr);
			assert(in_from_state->state_machine == in_to_state->state_machine);

			in_from_state->outgoing_transitions.push_back(this);
			in_to_state->incomming_transitions.push_back(this);
		}

		SM::StateAction Transition::CheckTransitionConditions(StateMachineInstance* sm_instance, Object * extra_data)
		{
			return SM::StateAction::MustStay;
		}

		void Transition::OnEnter(StateMachineInstance* sm_instance, StateBase* from_state, Object* extra_data)
		{
			StateAction action = OnEnterImpl(sm_instance, from_state, extra_data);
			if (action == StateAction::CanLeave)
				sm_instance->ChangeState(to_state, extra_data); // will cause OnLeave call
		}

		void Transition::Tick(StateMachineInstance* sm_instance, float delta_time, Object* extra_data)
		{
			StateAction action = TickImpl(sm_instance, delta_time, extra_data);
			if (action == StateAction::CanLeave)
				sm_instance->ChangeState(to_state, extra_data); // will cause OnLeave call
		}

		void Transition::OnLeave(StateMachineInstance* sm_instance, StateBase* to_state, Object* extra_data)
		{
			OnLeaveImpl(sm_instance, to_state, extra_data);
		}

		// ==================================================
		// StateMachine
		// ==================================================

		void StateMachine::SetInitialState(StateBase * in_state)
		{
			initial_state = in_state;
		}

		// ==================================================
		// StateMachineInstance
		// ==================================================

		StateMachineInstance::StateMachineInstance(StateMachine * in_state_machine) :
			state_machine(in_state_machine)
		{
			assert(in_state_machine != nullptr);
		}

		bool StateMachineInstance::Tick(float delta_time, Object * extra_data)
		{
			// enter initial state if necessary
			if (current_state == nullptr)
				ChangeState(state_machine->initial_state, extra_data);
			// nothing can be done if there is no current state
			if (current_state == nullptr)
				return false;
			// tick current state
			current_state->Tick(this, delta_time, extra_data);

			return true;
		}

		void StateMachineInstance::ChangeState(StateBase * new_state, Object * extra_data)
		{
			// early exit
			if (current_state == new_state)
				return;
			// leave previous state
			StateBase * old_state = current_state;
			if (old_state != nullptr)
				old_state->OnLeave(this, new_state, extra_data);
			// change the state
			current_state = new_state;
			// enter new state
			if (current_state != nullptr)
				current_state->OnEnter(this, old_state, extra_data);
		}

		void StateMachineInstance::Restart()
		{
			context_data = nullptr;
			ChangeState(state_machine->initial_state, nullptr);
		}

		bool StateMachineInstance::SendEvent(TagType event_tag, Object * extra_data)
		{
			if (current_state == nullptr)
				return false;
			return current_state->OnEventReceived(this, event_tag, extra_data);
		}

		State * StateMachineInstance::GetCurrentStrictState(bool use_destination)
		{
			if (current_state == nullptr)
				return nullptr;
			// the effective state
			State * result = auto_cast(current_state);
			if (result != nullptr)
				return result;
			// if in transition gets the starting position
			Transition * transition = auto_cast(current_state);
			if (transition != nullptr)
				return (use_destination) ? transition->to_state : transition->from_state;
			return nullptr;
		}

		State const * StateMachineInstance::GetCurrentStrictState(bool use_destination) const
		{
			if (current_state == nullptr)
				return nullptr;
			// the effective state
			State * result = auto_cast(current_state);
			if (result != nullptr)
				return result;
			// if in transition gets the starting position
			Transition * transition = auto_cast(current_state);
			if (transition != nullptr)
				return (use_destination)? transition->to_state : transition->from_state;
			return nullptr;
		}


	}; // namespace StateMachine

}; // namespace chaos
