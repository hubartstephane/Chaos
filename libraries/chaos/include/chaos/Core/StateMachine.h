#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	namespace SM
	{
		class StateBase;
		class State;
		class Transition;
		class StateMachine;
		class StateMachineInstance;

	}; // namespace SM
	
}; // namespace chaos

#else 

namespace chaos
{
	namespace SM // for StateMachine
	{

		// ==================================================
		// Forward declaration / friendship macros
		// ==================================================

		// all classes in this file
#define CHAOS_STATEMACHINE_CLASSES \
(StateBase) \
(State) \
(Transition) \
(StateMachine) \
(StateMachineInstance)

		// forward declaration
#define CHAOS_STATEMACHINE_FORWARD_DECL(r, data, elem) class elem;		
		BOOST_PP_SEQ_FOR_EACH(CHAOS_STATEMACHINE_FORWARD_DECL, _, CHAOS_STATEMACHINE_CLASSES)

			// friendship macro
#define CHAOS_STATEMACHINE_FRIEND_DECL(r, data, elem) friend class elem;
#define CHAOS_STATEMACHINE_ALL_FRIENDS BOOST_PP_SEQ_FOR_EACH(CHAOS_STATEMACHINE_FRIEND_DECL, _, CHAOS_STATEMACHINE_CLASSES)

			// ==================================================
			// State
			// ==================================================

		class StateBase : public Object, public NamedObject
		{
			CHAOS_STATEMACHINE_ALL_FRIENDS

		public:

			/** constructor */
			StateBase(StateMachine * in_state_machine);

		protected:

			/** FRAMEWORK : called at each tick */
			virtual void Tick(StateMachineInstance * sm_instance, float delta_time, Object * extra_data){}
			/** FRAMEWORK : called whenever we enter in this state */
			virtual void OnEnter(StateMachineInstance * sm_instance, StateBase * from_state, Object * extra_data){}
			/** FRAMEWORK : called whenever we leave this state */
			virtual void OnLeave(StateMachineInstance * sm_instance, StateBase * to_state, Object * extra_data){}
			/** FRAMEWORK : called whenever an event is send to the StateMachineInstance */
			virtual bool SendEvent(StateMachineInstance * sm_instance, TagType event_tag, Object * extra_data){ return false; }

			/** USER IMPLEMENTATION : called at each tick */
			virtual bool TickImpl(StateMachineInstance * sm_instance, float delta_time, Object * extra_data) { return true; }
			/** USER IMPLEMENTATION : called whenever we enter in this state */
			virtual bool OnEnterImpl(StateMachineInstance * sm_instance, StateBase * from_state, Object * extra_data){ return true;}
			/** USER IMPLEMENTATION : called whenever we leave this state */
			virtual bool OnLeaveImpl(StateMachineInstance * sm_instance, StateBase * to_state, Object * extra_data){ return true; }
			/** USER IMPLEMENTATION : called whenever an event is send to the StateMachineInstance */
			virtual bool SendEventImpl(StateMachineInstance * sm_instance, TagType event_tag, Object * extra_data){ return false; }

		protected:

			/** the state_machine this instance belongs to */
			StateMachine * state_machine = nullptr;
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

			/** FRAMEWORK : called at each tick. Returns true if outgoing transition can be tested */
			virtual void Tick(StateMachineInstance * sm_instance, float delta_time, Object * extra_data) override;
			/** FRAMEWORK : called whenever we enter in this state */
			virtual void OnEnter(StateMachineInstance * sm_instance, StateBase * from_state, Object * extra_data) override;
			/** FRAMEWORK : called whenever we leave this state */
			virtual void OnLeave(StateMachineInstance * sm_instance, StateBase * to_state, Object * extra_data) override;
			/** FRAMEWORK : called whenever an event is send to the StateMachineInstance */
			virtual bool SendEvent(StateMachineInstance * sm_instance, TagType event_tag, Object * extra_data) override;

			/** USER IMPLEMENTATION : called at each tick. Returns true if outgoing transition can be tested */
			virtual bool TickImpl(StateMachineInstance * sm_instance, float delta_time, Object * extra_data) override;
			/** USER IMPLEMENTATION : called whenever we enter in this state */
			virtual bool OnEnterImpl(StateMachineInstance * sm_instance, StateBase * from_state, Object * extra_data) override;
			/** USER IMPLEMENTATION : called whenever we leave this state */
			virtual bool OnLeaveImpl(StateMachineInstance * sm_instance, StateBase * to_state, Object * extra_data) override;
			/** USER IMPLEMENTATION : called whenever an event is send to the StateMachineInstance */
			virtual bool SendEventImpl(StateMachineInstance * sm_instance, TagType event_tag, Object * extra_data) override;

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
			Transition(State * in_from_state, State * in_to_state, TagType in_triggering_event = 0);

		protected:

			/** user implementable method to know whether the transition can trigger itself. Returns true whether the transition can be automatically used */
			virtual bool CheckTransitionConditions(Object * extra_data);

			/** FRAMEWORK : called at each tick. Returns true if outgoing transition can be tested */
			virtual void Tick(StateMachineInstance * sm_instance, float delta_time, Object * extra_data) override;
			/** FRAMEWORK : called whenever we enter in this state. Returns true whether this is a passthrought transition (directly go to outgoing transition) */
			virtual void OnEnter(StateMachineInstance * sm_instance, StateBase * from_state, Object * extra_data) override;
			/** FRAMEWORK : called whenever we leave this state */
			virtual void OnLeave(StateMachineInstance * sm_instance, StateBase * to_state, Object * extra_data) override;

			/** USER IMPLEMENTATION : called at each tick. Returns true if outgoing transition can be tested */
			virtual bool TickImpl(StateMachineInstance * sm_instance, float delta_time, Object * extra_data) override;
			/** USER IMPLEMENTATION : called whenever we enter in this state */
			virtual bool OnEnterImpl(StateMachineInstance * sm_instance, StateBase * from_state, Object * extra_data) override;
			/** USER IMPLEMENTATION : called whenever we leave this state */
			virtual bool OnLeaveImpl(StateMachineInstance * sm_instance, StateBase * to_state, Object * extra_data) override;

		protected:

			/** initial state */
			State * from_state = nullptr;
			/** destination state */
			State * to_state = nullptr;
			/** the ID of the event that will automatically trigger the transition */
			TagType triggering_event = 0;
		};

		// ==================================================
		// StateMachineInstance
		// ==================================================

		class StateMachineInstance : public Object
		{
			CHAOS_STATEMACHINE_ALL_FRIENDS

		public:

			/** constructor */
			StateMachineInstance(StateMachine * in_state_machine);

			/** the tick method */
			bool Tick(float delta_time, Object * extra_data);
			/** restart the state_machine */
			void Restart();
			/** send an event to current state */
			bool SendEvent(TagType event_tag, Object * extra_data);

			/** get the current state (this may return a transition) */
			StateBase * GetCurrentState() { return current_state; }
			/** get the current state (this may return a transition) */
			StateBase const * GetCurrentState() const { return current_state; }

			/** get the current state (if in transition, this returns previous state) */
			State * GetCurrentStrictState(bool use_destination = false);
			/** get the current state (if in transition, this returns previous state) */
			State const * GetCurrentStrictState(bool use_destination = false) const;

			/** get the context object */
			Object * GetContextData(){ return context_data.get(); }
			/** set the context object */
			void SetContextData(Object * new_context_data){ context_data = new_context_data; }

		protected:

			/** internal method to change state */
			void ChangeState(StateBase * new_state, Object * extra_data);

		protected:

			/** the state machine */
			StateMachine * state_machine = nullptr;
			/** the current state of the state_machine */
			StateBase * current_state = nullptr;
			/** some data for current state / transition */
			shared_ptr<Object> context_data;
		};

		// ==================================================
		// StateMachine
		// ==================================================

		class StateMachine : public Object
		{
			CHAOS_STATEMACHINE_ALL_FRIENDS

			CHAOS_DECLARE_OBJECT_CLASS2(StateMachine, Object);

		public:

			/** the initialization method */
			virtual bool Initialize() { return true; }

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

#endif CHAOS_FORWARD_DECLARATION


