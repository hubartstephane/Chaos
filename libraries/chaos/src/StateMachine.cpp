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

		void StateBase::SetName(char const * in_name)
		{
			if (in_name == nullptr)
				name.clear();
			else
				name = in_name;
		}


		// ==================================================
		// State
		// ==================================================

		State::State(StateMachine * in_state_machine):
			StateBase(in_state_machine)
		{
		}

		void State::Tick(double delta_time, StateMachineInstance * sm_instance)
		{
			// the USER implementation catch the method
			if (!TickImpl(delta_time, sm_instance))
				return;
			// give the outgoing transition the opportunities to trigger themselves
			for (Transition * transition : outgoing_transitions)
			{
				if (transition->CheckTransitionConditions()) // check outgoing transtitions
				{
					sm_instance->ChangeState(transition);
					return;
				}
			}
		}

		void State::OnEnter(StateBase * from_state, StateMachineInstance * sm_instance)
		{	
			OnEnterImpl(from_state, sm_instance);
		}

		void State::OnLeave(StateBase * to_state, StateMachineInstance * sm_instance)
		{		
			OnLeaveImpl(to_state, sm_instance);
		}

		bool State::SendEvent(int event_id, void * extra_data, StateMachineInstance * sm_instance)
		{
			// the USER implementation catch the method
			if (SendEventImpl(event_id, extra_data, sm_instance))
				return true;
			// give to the outgoing transitions the opportunities to catch the event
			for (Transition * transition : outgoing_transitions)
			{
				if (transition->triggering_event >= 0 && transition->triggering_event == event_id)
				{
					sm_instance->ChangeState(transition);
					return true;
				}
			}
			return false; // not handled
		}

		bool State::OnEnterImpl(StateBase * from_state, StateMachineInstance * sm_instance)
		{
			return false; 
		}

		bool State::TickImpl(double delta_time, StateMachineInstance * sm_instance)
		{
			return true; // enable automatic transitions
		}

		bool State::OnLeaveImpl(StateBase * to_state, StateMachineInstance * sm_instance)
		{
			return false;
		}

		bool State::SendEventImpl(int event_id, void * extra_data, StateMachineInstance * sm_instance)
		{
			return false; // do not catch the event : let the outgoing transition do their job
		}

		// ==================================================
		// Transition
		// ==================================================

		Transition::Transition(State * in_from_state, State * in_to_state, int in_triggering_event):
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

		bool Transition::CheckTransitionConditions()
		{
			return false; // refuse to automatically enter the transition
		}


		void Transition::OnEnter(StateBase * from_state, StateMachineInstance * sm_instance)
		{
			// test whether this is a passtrought transition
			if (OnEnterImpl(from_state, sm_instance))
				sm_instance->ChangeState(to_state); // will cause OnLeave call
		}

		void Transition::Tick(double delta_time, StateMachineInstance * sm_instance)
		{	
			if (TickImpl(delta_time, sm_instance))
				sm_instance->ChangeState(to_state); // will cause OnLeave call
		}

		void Transition::OnLeave(StateBase * to_state, StateMachineInstance * sm_instance)
		{
			OnLeaveImpl(to_state, sm_instance);
		}


		bool Transition::OnEnterImpl(StateBase * from_state, StateMachineInstance * sm_instance)
		{
			return true; // pass throught transition (no tick)
		}

		bool Transition::TickImpl(double delta_time, StateMachineInstance * sm_instance)
		{
			return true; // the transition has no special code. We can exit it
		}

		bool Transition::OnLeaveImpl(StateBase * to_state, StateMachineInstance * sm_instance)
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

		bool StateMachineInstance::Tick(double delta_time)
		{
			// enter initial state if necessary
			if (current_state == nullptr)
				ChangeState(state_machine->initial_state);
			// nothing can be done if there is no current state
			if (current_state == nullptr)
				return false;
			// tick current state
			current_state->Tick(delta_time, this);

			return true;
		}

		void StateMachineInstance::ChangeState(StateBase * new_state)
		{
			// early exit
			if (current_state == new_state)
				return;
			// leave previous state
			StateBase * old_state = current_state;
			if (old_state != nullptr)
				old_state->OnLeave(new_state, this);
			// change the state
			current_state = new_state;
			// enter new state
			if (current_state != nullptr)
				current_state->OnEnter(old_state, this);
		}

		void StateMachineInstance::Restart()
		{
			context_data = nullptr;
			ChangeState(state_machine->initial_state);
		}

		bool StateMachineInstance::SendEvent(int event_id, void * extra_data)
		{
			if (current_state == nullptr)
				return false;
			return current_state->SendEvent(event_id, extra_data, this);
		}


	}; // namespace StateMachine

}; // namespace chaos
