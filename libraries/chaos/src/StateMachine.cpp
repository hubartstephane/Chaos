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

		void State::Tick(double delta_time, StateMachineInstance * sm_instance, ReferencedObject * context_data)
		{
			if (TickImpl(delta_time, context_data))
			{
				for (Transition * transition : outgoing_transitions)
				{
					if (transition->CheckTransitionConditions()) // check outgoing transtitions
					{
						sm_instance->ChangeState(transition);
						return;
					}
				}
			}
		}

		void State::OnEnter(State * from_state, StateMachineInstance * sm_instance, ReferencedObject * context_data)
		{	
			OnEnterImpl(from_state, context_data);
		}

		void State::OnLeave(State * to_state, StateMachineInstance * sm_instance, ReferencedObject * context_data)
		{		
			OnLeaveImpl(to_state, context_data);
		}

		bool State::OnEnterImpl(State * from_state, ReferencedObject * context_data)
		{
			return false; 
		}

		bool State::TickImpl(double delta_time, ReferencedObject * context_data)
		{
			return true;
		}

		bool State::OnLeaveImpl(State * to_state, ReferencedObject * context_data)
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


		void Transition::OnEnter(State * from_state, StateMachineInstance * sm_instance, ReferencedObject * context_data)
		{
			if (OnEnterImpl(from_state, context_data))
				sm_instance->ChangeState(to_state); // will cause OnLeave call
		}

		void Transition::Tick(double delta_time, StateMachineInstance * sm_instance, ReferencedObject * context_data)
		{	
			if (TickImpl(delta_time, context_data))
				sm_instance->ChangeState(to_state); // will cause OnLeave call
		}

		void Transition::OnLeave(State * to_state, StateMachineInstance * sm_instance, ReferencedObject * context_data)
		{
			OnLeaveImpl(to_state, context_data);
		}


		bool Transition::OnEnterImpl(State * from_state, ReferencedObject * context_data)
		{
			return true; // pass throught transition (no tick)
		}

		bool Transition::TickImpl(double delta_time, ReferencedObject * context_data)
		{
			return true;
		}

		bool Transition::OnLeaveImpl(State * to_state, ReferencedObject * context_data)
		{
			return true;
		}

		// ==================================================
		// StateMachine
		// ==================================================

		void StateMachine::SetInitialState(State * in_state)
		{
			initial_state = in_state;	
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
			// enter initial state if necessary
			if (current_state == nullptr)
				ChangeState(state_machine->initial_state);
			// nothing can be done if there is no current state
			if (current_state == nullptr)
				return false;

			// tick current state
			current_state->Tick(delta_time, this, context_data.get());

			return true;
		}

		void StateMachineInstance::ChangeState(State * new_state)
		{
			// early exit
			if (current_state == new_state)
				return;
			// leave previous state
			State * old_state = current_state;
			if (old_state != nullptr)
				old_state->OnLeave(new_state, this, context_data.get());
			// change the state
			current_state = new_state;
			// enter new state
			if (current_state != nullptr)
				current_state->OnEnter(old_state, this, context_data.get());
		}

		void StateMachineInstance::Restart()
		{
			context_data = nullptr;
			ChangeState(state_machine->initial_state);
		}

		void StateMachineInstance::SendEvent(int event_id, void * extra_data)
		{

		}


	}; // namespace StateMachine

}; // namespace chaos
