namespace chaos
{

#ifdef CHAOS_FORWARD_DECLARATION

	class GameState;
	class GameTransition;
	class InitialState;
	class MainMenuState;
	class PlayingState;
	class PauseState;
	class InitialToMainMenuTransition;
	class MainMenuToPlayingTransition;
	class PlayingToMainMenuTransition;
	class PlayingToGameOverTransition;
	class GameStateMachine;
	class GameStateMachineInstance;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	namespace GameStateMachineKeys
	{
		// the possible states
		CHAOS_DECLARE_TAG(STATE_INITIAL);
		CHAOS_DECLARE_TAG(STATE_MAINMENU);
		CHAOS_DECLARE_TAG(STATE_PAUSE);
		CHAOS_DECLARE_TAG(STATE_PLAYING);

		// the events
		CHAOS_DECLARE_TAG(EVENT_TOGGLE_PAUSE);
		CHAOS_DECLARE_TAG(EVENT_EXIT_GAME);
		CHAOS_DECLARE_TAG(EVENT_START_GAME);
		CHAOS_DECLARE_TAG(EVENT_GAME_INITIALIZED);
		CHAOS_DECLARE_TAG(EVENT_GAME_OVER);
	};

	// =========================================================
	// GameState
	// =========================================================

	class CHAOS_API GameState : public SM::State
	{
	public:

		/** constructor */
		GameState(class GameStateMachine* in_state_machine);
		/** get the game */
		Game* GetGame(SM::StateMachineInstance* sm_instance);
		/** get the game */
		Game const* GetGame(SM::StateMachineInstance const* sm_instance) const;
	};

	// =========================================================
	// GameTransition
	// =========================================================

	class CHAOS_API GameTransition : public SM::Transition
	{
	public:

		/** constructor */
		GameTransition(GameState* in_from_state, GameState* in_to_state, TagType in_triggering_event);
		/** get the game */
		Game* GetGame(SM::StateMachineInstance* sm_instance);
		/** get the game */
		Game const* GetGame(SM::StateMachineInstance* sm_instance) const;
	};

	// =========================================================
	// All states
	// =========================================================

	class CHAOS_API InitialState : public GameState
	{
	public:

		/** constructor */
		InitialState(GameStateMachine* in_state_machine);
		/** override */
		virtual SM::StateAction OnEnterImpl(SM::StateMachineInstance* sm_instance, SM::StateBase* from_state, Object* extra_data) override;
	};

	class CHAOS_API MainMenuState : public GameState
	{
	public:

		/** constructor */
		MainMenuState(GameStateMachine* in_state_machine);
		/** override */
		virtual SM::StateAction OnEnterImpl(SM::StateMachineInstance* sm_instance, SM::StateBase* from_state, Object* extra_data) override;
		/** override */
		virtual void OnLeaveImpl(SM::StateMachineInstance* sm_instance, SM::StateBase* to, Object* extra_data) override;
	};

	class CHAOS_API PlayingState : public GameState
	{
	public:

		/** constructor */
		PlayingState(GameStateMachine* in_state_machine);
		/** override */
		virtual SM::StateAction TickImpl(SM::StateMachineInstance* sm_instance, float delta_time, Object* extra_data) override;
	};

	class CHAOS_API PauseState : public GameState
	{
	public:

		/** constructor */
		PauseState(GameStateMachine* in_state_machine);
		/** override */
		virtual SM::StateAction OnEnterImpl(SM::StateMachineInstance* sm_instance, SM::StateBase* from_state, Object* extra_data) override;
		/** override */
		virtual void OnLeaveImpl(SM::StateMachineInstance* sm_instance, SM::StateBase* to, Object* extra_data) override;
	};

	// =========================================================
	// Transitions
	// =========================================================

	class CHAOS_API InitialToMainMenuTransition : public GameTransition
	{
	public:

		using GameTransition::GameTransition; // inherit constructor

	protected:

		/** override */
		virtual SM::StateAction OnEnterImpl(SM::StateMachineInstance* sm_instance, SM::StateBase* from, Object* extra_data) override;
		/** override */
		virtual SM::StateAction CheckTransitionConditions(SM::StateMachineInstance* sm_instance, Object* extra_data) override;
	};

	class CHAOS_API MainMenuToPlayingTransition : public GameTransition
	{
	public:

		using GameTransition::GameTransition; // inherit constructor

	protected:

		/** override */
		virtual SM::StateAction OnEnterImpl(SM::StateMachineInstance* sm_instance, SM::StateBase* from, Object* extra_data) override;
	};

	class CHAOS_API PlayingToMainMenuTransition : public GameTransition
	{

	public:

		using GameTransition::GameTransition; // inherit constructor

	protected:

		/** override */
		virtual SM::StateAction OnEnterImpl(SM::StateMachineInstance* sm_instance, SM::StateBase* from, Object* extra_data) override;
	};

	class CHAOS_API PlayingToGameOverTransition : public GameTransition
	{

	public:

		using GameTransition::GameTransition; // inherit constructor

	protected:

		/** override */
		virtual SM::StateAction TickImpl(SM::StateMachineInstance* sm_instance, float delta_time, Object* extra_data) override;
		/** override */
		virtual SM::StateAction OnEnterImpl(SM::StateMachineInstance* sm_instance, SM::StateBase* from, Object* extra_data) override;
		/** override */
		virtual void OnLeaveImpl(SM::StateMachineInstance* sm_instance, SM::StateBase* to, Object* extra_data) override;
		
	};

	// =========================================================
	// GameStateMachine
	// =========================================================

	class CHAOS_API GameStateMachine : public SM::StateMachine
	{
		friend class Game;

		CHAOS_DECLARE_OBJECT_CLASS(GameStateMachine, SM::StateMachine);

	public:

		/** create states and transitions */
		virtual bool Initialize() override;

	public:

		/** the states */
		shared_ptr<GameState> initial_state;
		shared_ptr<GameState> main_menu_state;
		shared_ptr<GameState> playing_state;
		shared_ptr<GameState> pause_state;

		/** the transitions */
		shared_ptr<GameTransition> initial_to_main_menu;
		shared_ptr<GameTransition> main_menu_to_playing;
		shared_ptr<GameTransition> playing_to_main_menu;
		shared_ptr<GameTransition> playing_to_pause;
		shared_ptr<GameTransition> pause_to_playing;
		shared_ptr<GameTransition> playing_to_gameover;
	};

	// =========================================================
	// GameStateMachineInstance
	// =========================================================

	class CHAOS_API GameStateMachineInstance : public SM::StateMachineInstance
	{
	public:

		/** constructor */
		GameStateMachineInstance(Game* in_game, SM::StateMachine* in_state_machine);

		/** get the game */
		Game* GetGame()
		{
			return game;
		}
		/** get the game */
		Game const* GetGame() const
		{
			return game;
		}

	protected:

		/** the game */
		Game* game = nullptr;
	};

#endif

}; // namespace chaos