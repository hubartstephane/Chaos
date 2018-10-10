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
#define CHAOS_STATEMACHINE_CLASSES (StateBase) (State) (Transition) (StateMachine) (StateMachineInstance)

		// forward declaration
#define CHAOS_STATEMACHINE_FORWARD_DECL(r, data, elem) class elem;		
		BOOST_PP_SEQ_FOR_EACH(CHAOS_STATEMACHINE_FORWARD_DECL, _, CHAOS_STATEMACHINE_CLASSES)

			// friendship macro
#define CHAOS_STATEMACHINE_FRIEND_DECL(r, data, elem) friend class elem;
#define CHAOS_STATEMACHINE_ALL_FRIENDS BOOST_PP_SEQ_FOR_EACH(CHAOS_STATEMACHINE_FRIEND_DECL, _, CHAOS_STATEMACHINE_CLASSES)

			// ==================================================
			// State
			// ==================================================

		class StateBase : public ReferencedObject
		{
			CHAOS_STATEMACHINE_ALL_FRIENDS

		public:

			/** constructor */
			StateBase(StateMachine * in_state_machine);
			/** destructor */
			virtual ~StateBase();

			/** change the ID */
			void SetID(int new_id) { id = new_id; }
			/** get the ID */
			int GetID() const { return id; }

			/** change the name */
			void SetName(char const * in_name);
			/** get the name */
			char const * GetName() const { return name.c_str(); }

		protected:

			/** FRAMEWORK : called whenever we enter in this state */
			virtual void OnEnter(StateBase * from_state, StateMachineInstance * sm_instance){}
			/** FRAMEWORK : called at each tick */
			virtual void Tick(double delta_time, StateMachineInstance * sm_instance){}
			/** FRAMEWORK : called whenever we leave this state */
			virtual void OnLeave(StateBase * to_state, StateMachineInstance * sm_instance){}
			/** FRAMEWORK : called whenever an event is send to the StateMachineInstance */
			virtual bool SendEvent(int event_id, void * extra_data, StateMachineInstance * sm_instance){ return false; }

			/** USER IMPLEMENTATION : called whenever we enter in this state */
			virtual bool OnEnterImpl(StateBase * from_state, StateMachineInstance * sm_instance){ return true;}
			/** USER IMPLEMENTATION : called at each tick */
			virtual bool TickImpl(double delta_time, StateMachineInstance * sm_instance){ return true; }
			/** USER IMPLEMENTATION : called whenever we leave this state */
			virtual bool OnLeaveImpl(StateBase * to_state, StateMachineInstance * sm_instance){ return true; }
			/** USER IMPLEMENTATION : called whenever an event is send to the StateMachineInstance */
			virtual bool SendEventImpl(int event_id, void * extra_data, StateMachineInstance * sm_instance){ return false; }

		protected:

			/** the state_machine this instance belongs to */
			StateMachine * state_machine = nullptr;
			/** an ID for the state */
			int id = 0;
			/** the name of the state */
			std::string name;
		};


		// ==================================================
		// State
		// ==================================================

		class State : public StateBase
		{
			CHAOS_STATEMACHINE_ALL_FRIENDS

		public:

			/** constructor */
			State(StateMachine * in_state_machine);

		protected:

			/** FRAMEWORK : called whenever we enter in this state */
			virtual void OnEnter(StateBase * from_state, StateMachineInstance * sm_instance) override;
			/** FRAMEWORK : called at each tick. Returns true if outgoing transition can be tested */
			virtual void Tick(double delta_time, StateMachineInstance * sm_instance) override;
			/** FRAMEWORK : called whenever we leave this state */
			virtual void OnLeave(StateBase * to_state, StateMachineInstance * sm_instance) override;
			/** FRAMEWORK : called whenever an event is send to the StateMachineInstance */
			virtual bool SendEvent(int event_id, void * extra_data, StateMachineInstance * sm_instance) override;

			/** USER IMPLEMENTATION : called whenever we enter in this state */
			virtual bool OnEnterImpl(StateBase * from_state, StateMachineInstance * sm_instance) override;
			/** USER IMPLEMENTATION : called at each tick. Returns true if outgoing transition can be tested */
			virtual bool TickImpl(double delta_time, StateMachineInstance * sm_instance) override;
			/** USER IMPLEMENTATION : called whenever we leave this state */
			virtual bool OnLeaveImpl(StateBase * to_state, StateMachineInstance * sm_instance) override;
			/** USER IMPLEMENTATION : called whenever an event is send to the StateMachineInstance */
			virtual bool SendEventImpl(int event_id, void * extra_data, StateMachineInstance * sm_instance) override;

		protected:

			/** the list of outgoing transitions */
			std::vector<Transition *> outgoing_transitions;
			/** the list of incomming transitions */
			std::vector<Transition *> incomming_transitions;
		};

		// ==================================================
		// Transition
		// ==================================================

		class Transition : public StateBase
		{
			CHAOS_STATEMACHINE_ALL_FRIENDS

		public:

			/** constructor */
			Transition(State * in_from_state, State * in_to_state, int in_triggering_event = -1);

		protected:

			/** user implementable method to know whether the transition can trigger itself. Returns true whether the transition can be automatically used */
			virtual bool CheckTransitionConditions();

			/** FRAMEWORK : called whenever we enter in this state. Returns true whether this is a passthrought transition (directly go to outgoing transition) */
			virtual void OnEnter(StateBase * from_state, StateMachineInstance * sm_instance) override;
			/** FRAMEWORK : called at each tick. Returns true if outgoing transition can be tested */
			virtual void Tick(double delta_time, StateMachineInstance * sm_instance) override;
			/** FRAMEWORK : called whenever we leave this state */
			virtual void OnLeave(StateBase * to_state, StateMachineInstance * sm_instance) override;

			/** USER IMPLEMENTATION : called whenever we enter in this state */
			virtual bool OnEnterImpl(StateBase * from_state, StateMachineInstance * sm_instance) override;
			/** USER IMPLEMENTATION : called at each tick. Returns true if outgoing transition can be tested */
			virtual bool TickImpl(double delta_time, StateMachineInstance * sm_instance) override;
			/** USER IMPLEMENTATION : called whenever we leave this state */
			virtual bool OnLeaveImpl(StateBase * to_state, StateMachineInstance * sm_instance) override;

		protected:

			/** initial state */
			State * from_state = nullptr;
			/** destination state */
			State * to_state = nullptr;
			/** the ID of the event that will automatically trigger the transition */
			int triggering_event = -1;

		};

		// ==================================================
		// StateMachineInstance
		// ==================================================

		class StateMachineInstance : public ReferencedObject
		{
			CHAOS_STATEMACHINE_ALL_FRIENDS

		public:

			/** constructor */
			StateMachineInstance(StateMachine * in_state_machine);

			/** the tick method */
			bool Tick(double delta_time);
			/** restart the state_machine */
			void Restart();
			/** send an event to current state */
			bool SendEvent(int event_id, void * extra_data);

			/** get the current state */
			StateBase * GetCurrentState() { return current_state; }
			/** get the current state */
			StateBase const * GetCurrentState() const { return current_state; }


			/** get the context object */
			ReferencedObject * GetContextData(){ return context_data.get(); }
			/** set the context object */
			void SetContextData(ReferencedObject * new_context_data){ context_data = new_context_data; }

		protected:

			/** internal method to change state */
			void ChangeState(StateBase * new_state);

		protected:

			/** the state machine */
			StateMachine * state_machine = nullptr;
			/** the current state of the state_machine */
			StateBase * current_state = nullptr;
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

			/** create states and transitions */
			virtual bool InitializeStateMachine(){ return true; }

			/** set the initial state */
			void SetInitialState(StateBase * in_state);

		protected:

			/** the initial state of the state_machine */
			StateBase * initial_state = nullptr;
		};

		// undefine macros
#undef CHAOS_STATEMACHINE_CLASSES
#undef CHAOS_STATEMACHINE_FORWARD_DECL
#undef CHAOS_STATEMACHINE_FRIEND_DECL
#undef CHAOS_STATEMACHINE_ALL_FRIENDS


	}; // namespace StateMachine

}; // namespace chaos


