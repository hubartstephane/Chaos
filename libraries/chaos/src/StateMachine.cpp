#include <chaos/StateMachine.h>
#include <chaos/ReferencedObject.h>

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

		StateBase::~StateBase()
		{
		}

		// ==================================================
		// State
		// ==================================================

		State::State(StateMachine * in_state_machine):
			StateBase(in_state_machine)
		{
		}

		void State::Tick(StateMachineInstance * sm_instance, float delta_time, ReferencedObject * extra_data)
		{
			// the USER implementation catch the method
			if (!TickImpl(sm_instance, delta_time, extra_data))
				return;
			// give the outgoing transition the opportunities to trigger themselves
			for (Transition * transition : outgoing_transitions)
			{
				if (transition->CheckTransitionConditions(extra_data)) // check outgoing transtitions
				{
					sm_instance->ChangeState(transition, extra_data);
					return;
				}
			}
		}

		void State::OnEnter(StateMachineInstance * sm_instance, StateBase * from_state, ReferencedObject * extra_data)
		{	
			OnEnterImpl(sm_instance, from_state, extra_data);
		}

		void State::OnLeave(StateMachineInstance * sm_instance, StateBase * to_state, ReferencedObject * extra_data)
		{		
			OnLeaveImpl(sm_instance, to_state, extra_data);
		}

		bool State::SendEvent(StateMachineInstance * sm_instance, TagType event_tag, ReferencedObject * extra_data)
		{
			// the USER implementation catch the method
			if (SendEventImpl(sm_instance, event_tag, extra_data))
				return true;
			// give to the outgoing transitions the opportunities to catch the event
			for (Transition * transition : outgoing_transitions)
			{
				if (transition->triggering_event > 0 && transition->triggering_event == event_tag)
				{
					sm_instance->ChangeState(transition, extra_data);
					return true;
				}
			}
			return false; // not handled
		}

		bool State::OnEnterImpl(StateMachineInstance * sm_instance, StateBase * from_state, ReferencedObject * extra_data)
		{
			return false; 
		}

		bool State::TickImpl(StateMachineInstance * sm_instance, float delta_time, ReferencedObject * extra_data)
		{
			return true; // enable automatic transitions
		}

		bool State::OnLeaveImpl(StateMachineInstance * sm_instance, StateBase * to_state, ReferencedObject * extra_data)
		{
			return false;
		}

		bool State::SendEventImpl(StateMachineInstance * sm_instance, TagType event_tag, ReferencedObject * extra_data)
		{
			return false; // do not catch the event : let the outgoing transition do their job
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

		bool Transition::CheckTransitionConditions(ReferencedObject * extra_data)
		{
			return false; // refuse to automatically enter the transition
		}


		void Transition::OnEnter(StateMachineInstance * sm_instance, StateBase * from_state, ReferencedObject * extra_data)
		{
			// test whether this is a passtrought transition
			if (OnEnterImpl(sm_instance, from_state, extra_data))
				sm_instance->ChangeState(to_state, extra_data); // will cause OnLeave call
		}

		void Transition::Tick(StateMachineInstance * sm_instance, float delta_time, ReferencedObject * extra_data)
		{	
			if (TickImpl(sm_instance, delta_time, extra_data))
				sm_instance->ChangeState(to_state, extra_data); // will cause OnLeave call
		}

		void Transition::OnLeave(StateMachineInstance * sm_instance, StateBase * to_state, ReferencedObject * extra_data)
		{
			OnLeaveImpl(sm_instance, to_state, extra_data);
		}


		bool Transition::OnEnterImpl(StateMachineInstance * sm_instance, StateBase * from_state, ReferencedObject * extra_data)
		{
			return true; // pass throught transition (no tick)
		}

		bool Transition::TickImpl(StateMachineInstance * sm_instance, float delta_time, ReferencedObject * extra_data)
		{
			return true; // the transition has no special code. We can exit it
		}

		bool Transition::OnLeaveImpl(StateMachineInstance * sm_instance, StateBase * to_state, ReferencedObject * extra_data)
		{
			return true;
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

		bool StateMachineInstance::Tick(float delta_time, ReferencedObject * extra_data)
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

		void StateMachineInstance::ChangeState(StateBase * new_state, ReferencedObject * extra_data)
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

		bool StateMachineInstance::SendEvent(TagType event_tag, ReferencedObject * extra_data)
		{
			if (current_state == nullptr)
				return false;
			return current_state->SendEvent(this, event_tag, extra_data);
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
