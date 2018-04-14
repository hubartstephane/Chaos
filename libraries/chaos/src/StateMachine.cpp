#include <chaos/StateMachine.h>
#include <chaos/ReferencedObject.h>

namespace chaos
{
	namespace StateMachine
	{

		// ==================================================
		// State
		// ==================================================

		State::State(Automata * in_automata):
			automata(in_automata)
		{
			assert(in_automata != nullptr);		
		}

		State::~State()
		{
		
		}

		void State::Tick(double delta_time)
		{
			if (TickImpl(delta_time))
			{
				for (auto it : outgoing_transitions)
				{
					if (it->CheckTransitionConditions()) // check outgoing transtitions
					{
						automata->ChangeState(it);
						return;
					}
				}
			}
		}

		void State::OnEnter(State * from_state)
		{	
			OnEnterImpl(from_state);
		}

		void State::OnLeave(State * to_state)
		{		
			OnLeaveImpl(to_state);
		}
	
		bool State::OnEnterImpl(State * from_state)
		{
			return false; 
		}

		bool State::TickImpl(double delta_time)
		{
			return true;
		}
		
		bool State::OnLeaveImpl(State * to_state)
		{
			return false;
		}

		// ==================================================
		// Transition
		// ==================================================

		Transition::Transition(State * in_from_state, State * in_to_state):
			State(in_from_state->automata),
			from_state(in_from_state),
			to_state(in_to_state)
		{
			assert(in_from_state != nullptr);
			assert(in_to_state != nullptr);
			assert(in_from_state->automata == in_to_state->automata);

			in_from_state->outgoing_transitions.push_back(this);
			in_to_state->incomming_transitions.push_back(this);
		}

		bool Transition::TriggerTransition(bool force)
		{
			// triggering the transition is only possible if the automata is in start state
			if (automata->current_state != from_state)
				return false;
			// test for conditions if required
			if (!force && !CheckTransitionConditions())
				return false;
			//change the state
			automata->ChangeState(to_state);		
			return true;
		}

		bool Transition::CheckTransitionConditions()
		{
		


			return false; // refuse to automatically enter the transition
		}


		void Transition::OnEnter(State * from_state)
		{
			if (OnEnterImpl(from_state))
				automata->ChangeState(to_state); // will cause OnLeave call
		}

		void Transition::Tick(double delta_time)
		{	
			if (TickImpl(delta_time))
				automata->ChangeState(to_state); // will cause OnLeave call
		}

		void Transition::OnLeave(State * to_state)
		{
			OnLeaveImpl(to_state);
		}

		// ==================================================
		// Automata
		// ==================================================

		bool Automata::Tick(double delta_time, int max_transition_changes)
		{
			// enter initial state if necessary
			if (current_state == nullptr)
				ChangeState(initial_state);
			// nothing can be done if there is no current state
			if (current_state == nullptr)
				return false;

			// tick current state
			current_state->Tick(delta_time);

			return true;
		}

		void Automata::ChangeState(State * new_state)
		{
			// early exit
			if (current_state == new_state)
				return;
			// leave previous state
			State * old_state = current_state; 
			if (old_state != nullptr)
				old_state->OnLeave(new_state);
			// change the state
			current_state = new_state;
			// enter new state
			if (current_state != nullptr)
				current_state->OnEnter(old_state);
		}

		void Automata::SetInitialState(State * in_state)
		{
			initial_state = in_state;	
		}

		void Automata::Restart()
		{
			ChangeState(initial_state);
		}

	}; // namespace chaos

}; // namespace chaos
