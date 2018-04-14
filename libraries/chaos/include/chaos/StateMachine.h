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

			/** constructor */
			State(Automata * in_automata);
			/** destructor */
			virtual ~State();

			/** change the ID */
			void SetStateID(int new_id) { id = new_id; }
			/** get the ID */
			int GetStateID() const { return id; }

		protected: 

			/** FRAMEWORK : called whenever we enter in this state */
			virtual void OnEnter(State * from_state);
			/** FRAMEWORK : called at each tick. Returns true if outgoing transition can be tested */
			virtual void Tick(double delta_time);
			/** FRAMEWORK : called whenever we leave this state */
			virtual void OnLeave(State * to_state);

			/** USER IMPLEMENTATION : called whenever we enter in this state */
			virtual bool OnEnterImpl(State * from_state);
			/** USER IMPLEMENTATION : called at each tick. Returns true if outgoing transition can be tested */
			virtual bool TickImpl(double delta_time);
			/** USER IMPLEMENTATION : called whenever we leave this state */
			virtual bool OnLeaveImpl(State * to_state);

		protected:

			/** the automata this instance belongs to */
			Automata * automata = nullptr;

			/** an ID for the state */
			int id = 0;
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

			/** force trigger the state change */
			bool TriggerTransition(bool force = true);

		protected:

			/** user implementable method to know whether the transition can trigger itself */
			virtual bool CheckTransitionConditions();

			/** FRAMEWORK : called whenever we enter in this state */
			virtual void OnEnter(State * from_state) override;
			/** FRAMEWORK : called at each tick. Returns true if outgoing transition can be tested */
			virtual void Tick(double delta_time) override;
			/** FRAMEWORK : called whenever we leave this state */
			virtual void OnLeave(State * to_state) override;

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

			/** restart the automata */
			void Restart();

			/** get the current state */
			State * GetCurrentState() { return current_state; }
			/** get the current state */
			State const * GetCurrentState() const { return current_state; }

		protected:

			/** internal method to change state */
			void ChangeState(State * new_state);
			/** set the initial state */
			void SetInitialState(State * in_state);

		protected:

			/** the initial state of the automata */
			State * initial_state = nullptr;

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


