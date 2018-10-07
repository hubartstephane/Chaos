#include <chaos/StateMachine.h>
#include <chaos/ReferencedObject.h>

namespace chaos
{
	namespace SM
	{

		// ==================================================
		// State
		// ==================================================

		State::State(StateMachine * in_state_machine):
			state_machine(in_state_machine)
		{
			assert(in_state_machine != nullptr);		
		}

		State::~State()
		{

		}

		void State::SetName(char const * in_name)
		{
			if (in_name == nullptr)
				name.clear();
			else
				name = in_name;
		}

		void State::Tick(double delta_time)
		{
			if (TickImpl(delta_time))
			{
				for (auto it : outgoing_transitions)
				{
					if (it->CheckTransitionConditions()) // check outgoing transtitions
					{
						state_machine->ChangeState(it);
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
			State(in_from_state->state_machine),
			from_state(in_from_state),
			to_state(in_to_state)
		{
			assert(in_from_state != nullptr);
			assert(in_to_state != nullptr);
			assert(in_from_state->state_machine == in_to_state->state_machine);

			in_from_state->outgoing_transitions.push_back(this);
			in_to_state->incomming_transitions.push_back(this);
		}

		bool Transition::TriggerTransition(bool force)
		{
			// triggering the transition is only possible if the state_machine is in start state
			if (state_machine->current_state != from_state)
				return false;
			// test for conditions if required
			if (!force && !CheckTransitionConditions())
				return false;
			// change the state
			state_machine->ChangeState(this);		
			return true;
		}

		bool Transition::CheckTransitionConditions()
		{



			return false; // refuse to automatically enter the transition
		}


		void Transition::OnEnter(State * from_state)
		{
			if (OnEnterImpl(from_state))
				state_machine->ChangeState(to_state); // will cause OnLeave call
		}

		void Transition::Tick(double delta_time)
		{	
			if (TickImpl(delta_time))
				state_machine->ChangeState(to_state); // will cause OnLeave call
		}

		void Transition::OnLeave(State * to_state)
		{
			OnLeaveImpl(to_state);
		}


		bool Transition::OnEnterImpl(State * from_state)
		{
			return true; // pass throught transition (no tick)
		}

		bool Transition::TickImpl(double delta_time)
		{
			return true;
		}

		bool Transition::OnLeaveImpl(State * to_state)
		{
			return true;
		}

		// ==================================================
		// StateMachine
		// ==================================================

		bool StateMachine::Tick(double delta_time)
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

		void StateMachine::ChangeState(State * new_state)
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

		void StateMachine::SetInitialState(State * in_state)
		{
			initial_state = in_state;	
		}

		void StateMachine::Restart()
		{
			ChangeState(initial_state);
		}

		StateMachineInstance * StateMachine::CreateInstance()
		{
			return new StateMachineInstance(this);
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

			
			return true;
		}

		void StateMachineInstance::Restart(int initial_state_id)
		{

		}

		void StateMachineInstance::SendEvent(int event_id, void * extra_data)
		{

		}


	}; // namespace StateMachine

}; // namespace chaos
