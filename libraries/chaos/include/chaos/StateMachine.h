#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>




namespace chaos
{
	namespace StateMachine
	{

		// ==================================================
		// Forward declaration / friendship macros
		// ==================================================

		// all classes in this file
#define CHAOS_STATEMACHINE_CLASSES (State) (Transition) (Automata)

		// forward declaration
#define CHAOS_STATEMACHINE_FORWARD_DECL(r, data, elem) class elem;
BOOST_PP_SEQ_FOR_EACH(CHAOS_STATEMACHINE_FORWARD_DECL, _, CHAOS_STATEMACHINE_CLASSES)

			// friendship macro
#define CHAOS_STATEMACHINE_FRIEND_DECL(r, data, elem) friend class elem;
#define CHAOS_STATEMACHINE_ALL_FRIENDS BOOST_PP_SEQ_FOR_EACH(CHAOS_STATEMACHINE_FRIEND_DECL, _, CHAOS_STATEMACHINE_CLASSES)

		// ==================================================
		// State
		// ==================================================

		class State : public ReferencedObject
		{
			CHAOS_STATEMACHINE_ALL_FRIENDS

		public:

			/** destructor */
			virtual ~State() = default;

			/** called whenever we enter in this state */
			virtual bool OnEnter(State * from_state);
			/** called at each tick. Returns true if outgoing transition can be tested */
			virtual bool Tick(double delta_time);
			/** called whenever we leave this state */
			virtual bool OnLeave(State * to_state);

		protected:

			/** the list of outgoing transitions */
			std::vector<Transition *> outgoing_transitions;
			/** the list of incomming transitions */
			std::vector<Transition *> incomming_transitions;
		};

		// ==================================================
		// Transition
		// ==================================================

		class Transition : public State
		{
			CHAOS_STATEMACHINE_ALL_FRIENDS

		public:

			/** constructor */
			Transition(State * in_from_state, State * in_to_state);

			/** trigger the state change */
			bool TriggerTransition();


		protected:

			/** initial state */
			State * from_state = nullptr;
			/** destination state */
			State * to_state = nullptr;

		};

		// ==================================================
		// Automata
		// ==================================================

		class Automata : public ReferencedObject
		{
			CHAOS_STATEMACHINE_ALL_FRIENDS

		public:

			/** destructor */
			virtual ~Automata() = default;

			/** the tick method */
			bool Tick(double delta_time, int max_transition_changes = 0);

		protected:

			/** the current state of the automata */
			State * current_state = nullptr;
		};

		// undefine macros
#undef CHAOS_STATEMACHINE_CLASSES
#undef CHAOS_STATEMACHINE_FORWARD_DECL
#undef CHAOS_STATEMACHINE_FRIEND_DECL
#undef CHAOS_STATEMACHINE_ALL_FRIENDS


	}; // namespace chaos

}; // namespace chaos


