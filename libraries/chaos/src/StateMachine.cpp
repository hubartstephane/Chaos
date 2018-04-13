#include <chaos/StateMachine.h>
#include <chaos/ReferencedObject.h>

namespace chaos
{
	namespace StateMachine
	{

		// ==================================================
		// State
		// ==================================================

		bool State::OnEnter(State * from_state)
		{
		
		
			return true;
		}

		bool State::Tick(double delta_time)
		{
		
		
			return true;
		}

		bool State::OnLeave(State * to_state)
		{
		
			return true;		
		}

		// ==================================================
		// Transition
		// ==================================================

		Transition::Transition(State * in_from_state, State * in_to_state):
			from_state(in_from_state),
			to_state(in_to_state)
		{
			assert(in_from_state != nullptr);
			assert(in_to_state != nullptr);

			in_from_state->outgoing_transitions.push_back(this);
			in_to_state->incomming_transitions.push_back(this);
		}

		bool Transition::TriggerTransition()
		{
		
		
			return true;
		}


		// ==================================================
		// Automata
		// ==================================================

		bool Automata::Tick(double delta_time, int max_transition_changes)
		{
			// nothing can be done if there is no current state
			if (current_state == nullptr)
				return false;
		
		
		
			return true;
		}



	}; // namespace chaos

}; // namespace chaos
