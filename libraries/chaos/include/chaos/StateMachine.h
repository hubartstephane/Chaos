#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>




namespace chaos
{
	namespace SM // for StateMachine
	{

		// ==================================================
		// Forward declaration / friendship macros
		// ==================================================

		// all classes in this file
#define CHAOS_STATEMACHINE_CLASSES (State) (Transition) (StateMachine) (StateMachineInstance)

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
			State(StateMachine * in_state_machine);
			/** destructor */
			virtual ~State();

			/** change the ID */
			void SetStateID(int new_id) { id = new_id; }
			/** get the ID */
			int GetStateID() const { return id; }

			/** change the name */
			void SetName(char const * in_name);
			/** get the name */
			char const * GetName() const { return name.c_str(); }

		protected:

			/** FRAMEWORK : called whenever we enter in this state */
			virtual void OnEnter(State * from_state, StateMachineInstance * sm_instance, ReferencedObject * context_data);
			/** FRAMEWORK : called at each tick. Returns true if outgoing transition can be tested */
			virtual void Tick(double delta_time, StateMachineInstance * sm_instance, ReferencedObject * context_data);
			/** FRAMEWORK : called whenever we leave this state */
			virtual void OnLeave(State * to_state, StateMachineInstance * sm_instance, ReferencedObject * context_data);

			/** USER IMPLEMENTATION : called whenever we enter in this state */
			virtual bool OnEnterImpl(State * from_state, ReferencedObject * context_data);
			/** USER IMPLEMENTATION : called at each tick. Returns true if outgoing transition can be tested */
			virtual bool TickImpl(double delta_time, ReferencedObject * context_data);
			/** USER IMPLEMENTATION : called whenever we leave this state */
			virtual bool OnLeaveImpl(State * to_state, ReferencedObject * context_data);

		protected:

			/** the state_machine this instance belongs to */
			StateMachine * state_machine = nullptr;

			/** an ID for the state */
			int id = 0;
			/** the name of the state */
			std::string name;

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
			virtual void OnEnter(State * from_state, StateMachineInstance * sm_instance, ReferencedObject * context_data) override;
			/** FRAMEWORK : called at each tick. Returns true if outgoing transition can be tested */
			virtual void Tick(double delta_time, StateMachineInstance * sm_instance, ReferencedObject * context_data) override;
			/** FRAMEWORK : called whenever we leave this state */
			virtual void OnLeave(State * to_state, StateMachineInstance * sm_instance, ReferencedObject * context_data) override;

			/** USER IMPLEMENTATION : called whenever we enter in this state */
			virtual bool OnEnterImpl(State * from_state, ReferencedObject * context_data) override;
			/** USER IMPLEMENTATION : called at each tick. Returns true if outgoing transition can be tested */
			virtual bool TickImpl(double delta_time, ReferencedObject * context_data) override;
			/** USER IMPLEMENTATION : called whenever we leave this state */
			virtual bool OnLeaveImpl(State * to_state, ReferencedObject * context_data) override;

		protected:

			/** initial state */
			State * from_state = nullptr;
			/** destination state */
			State * to_state = nullptr;

		};

		// ==================================================
		// StateMachineInstance
		// ==================================================

		class StateMachineInstance : public ReferencedObject
		{
			CHAOS_STATEMACHINE_ALL_FRIENDS

		protected:

			/** protected constructor */
			StateMachineInstance(StateMachine * in_state_machine);

		public:

			/** the tick method */
			bool Tick(double delta_time);
			/** restart the state_machine */
			void Restart();

			/** get the current state */
			State * GetCurrentState() { return current_state; }
			/** get the current state */
			State const * GetCurrentState() const { return current_state; }

			/** send an event to current state */
			void SendEvent(int event_id, void * extra_data);

		protected:

			/** internal method to change state */
			void ChangeState(State * new_state);

		protected:

			/** the state machine */
			StateMachine * state_machine = nullptr;
			/** the current state of the state_machine */
			State * current_state = nullptr;
			/** some data for current state / transition */
			boost::intrusive_ptr<ReferencedObject> context_data;
		};

		// ==================================================
		// StateMachine
		// ==================================================

		class StateMachine : public ReferencedObject
		{
			CHAOS_STATEMACHINE_ALL_FRIENDS

		public:

			/** destructor */
			virtual ~StateMachine() = default;

			/** create the context */
			StateMachineInstance * CreateInstance();

		protected:

			/** set the initial state */
			void SetInitialState(State * in_state);

		protected:

			/** the initial state of the state_machine */
			State * initial_state = nullptr;
		};

		// undefine macros
#undef CHAOS_STATEMACHINE_CLASSES
#undef CHAOS_STATEMACHINE_FORWARD_DECL
#undef CHAOS_STATEMACHINE_FRIEND_DECL
#undef CHAOS_STATEMACHINE_ALL_FRIENDS


	}; // namespace StateMachine

}; // namespace chaos


