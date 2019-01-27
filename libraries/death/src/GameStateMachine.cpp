#include <death/GameStateMachine.h>

namespace death
{

	// =========================================================
	// GameState
	// =========================================================

	GameState::GameState(GameStateMachine * in_state_machine) :
		chaos::SM::State(in_state_machine)
	{
	}

	Game * GameState::GetGame(chaos::SM::StateMachineInstance * sm_instance)
	{
		GameStateMachineInstance * game_state_machine_instance = dynamic_cast<GameStateMachineInstance *>(sm_instance);
		if (game_state_machine_instance != nullptr)
			return game_state_machine_instance->GetGame();
		return nullptr;
	}

	Game const * GameState::GetGame(chaos::SM::StateMachineInstance const * sm_instance) const
	{
		GameStateMachineInstance const * game_state_machine_instance = dynamic_cast<GameStateMachineInstance const *>(sm_instance);
		if (game_state_machine_instance != nullptr)
			return game_state_machine_instance->GetGame();
		return nullptr;
	}

	// =========================================================
	// GameTransition
	// =========================================================

	GameTransition::GameTransition(GameState * in_from_state, GameState * in_to_state, chaos::TagType in_triggering_event) :
		chaos::SM::Transition(in_from_state, in_to_state, in_triggering_event)
	{
	}

	Game * GameTransition::GetGame(chaos::SM::StateMachineInstance * sm_instance)
	{
		GameStateMachineInstance * game_state_machine_instance = dynamic_cast<GameStateMachineInstance *>(sm_instance);
		if (game_state_machine_instance != nullptr)
			return game_state_machine_instance->GetGame();
		return nullptr;
	}

	Game const * GameTransition::GetGame(chaos::SM::StateMachineInstance * sm_instance) const
	{
		GameStateMachineInstance const * game_state_machine_instance = dynamic_cast<GameStateMachineInstance const *>(sm_instance);
		if (game_state_machine_instance != nullptr)
			return game_state_machine_instance->GetGame();
		return nullptr;
	}

	// =========================================================
	// All states
	// =========================================================

	MainMenuState::MainMenuState(GameStateMachine * in_state_machine) :
		GameState(in_state_machine)
	{
		SetTag(GameStateMachineKeys::STATE_MAINMENU);
		SetName("MainMenu");
	}

	bool MainMenuState::OnEnterImpl(chaos::SM::StateMachineInstance * sm_instance, chaos::SM::StateBase * from, chaos::ReferencedObject * extra_data)
	{
		Game * game = GetGame(sm_instance);
		if (game != nullptr)
			game->OnEnterMainMenu(from == nullptr); // very first game ?
		return false;
	}

	PlayingState::PlayingState(GameStateMachine * in_state_machine) :
		GameState(in_state_machine)
	{
		SetTag(GameStateMachineKeys::STATE_PLAYING);
		SetName("Playing");
	}

	bool PlayingState::TickImpl(chaos::SM::StateMachineInstance * sm_instance, double delta_time, chaos::ReferencedObject * extra_data)
	{
		Game * game = GetGame(sm_instance);
		if (game != nullptr)
			game->TickGameLoop(delta_time);
		return true;
	}

	PauseState::PauseState(GameStateMachine * in_state_machine) :
		GameState(in_state_machine)
	{
		SetTag(GameStateMachineKeys::STATE_PAUSE);
		SetName("Pause");
	}

	// =========================================================
	// All transitions
	// =========================================================

	MainMenuToPlayingTransition::MainMenuToPlayingTransition(GameState * in_from_state, GameState * in_to_state, chaos::TagType in_triggering_event) :
		GameTransition(in_from_state, in_to_state, in_triggering_event)
	{
	}

	bool MainMenuToPlayingTransition::OnEnterImpl(chaos::SM::StateMachineInstance * sm_instance, chaos::SM::StateBase * from, chaos::ReferencedObject * extra_data)
	{
		Game * game = GetGame(sm_instance);
		if (game == nullptr)
			return true;

		PhysicalGamepadWrapper * wrapper = dynamic_cast<PhysicalGamepadWrapper*>(extra_data); 		// try get the physical gamepad

		game->OnEnterGame((wrapper == nullptr)? nullptr : wrapper->data);
		return false;
	}

	bool MainMenuToPlayingTransition::TickImpl(chaos::SM::StateMachineInstance * sm_instance, double delta_time, chaos::ReferencedObject * extra_data)
	{
		Game * game = GetGame(sm_instance);
		if (game == nullptr)
			return true;
		return game->IsGameEnterComplete();
	}

	PlayingToMainMenuTransition::PlayingToMainMenuTransition(GameState * in_from_state, GameState * in_to_state, chaos::TagType in_triggering_event) :
		GameTransition(in_from_state, in_to_state, in_triggering_event)
	{
	}

	bool PlayingToMainMenuTransition::OnEnterImpl(chaos::SM::StateMachineInstance * sm_instance, chaos::SM::StateBase * from, chaos::ReferencedObject * extra_data)
	{
		Game * game = GetGame(sm_instance);
		if (game == nullptr)
			return true;
		game->OnLeaveGame();
		return false;
	}

	bool PlayingToMainMenuTransition::TickImpl(chaos::SM::StateMachineInstance * sm_instance, double delta_time, chaos::ReferencedObject * extra_data)
	{
		Game * game = GetGame(sm_instance);
		if (game == nullptr)
			return true;
		return game->IsGameLeaveComplete();
	}

	bool PlayingToMainMenuTransition::OnLeaveImpl(chaos::SM::StateMachineInstance * sm_instance, chaos::SM::StateBase * to, chaos::ReferencedObject * extra_data)
	{
		Game * game = GetGame(sm_instance);
		if (game == nullptr)
			return true;
		game->OnAbordGame();
		return false;
	}

	PlayingToPauseTransition::PlayingToPauseTransition(GameState * in_from_state, GameState * in_to_state, chaos::TagType in_triggering_event) :
		GameTransition(in_from_state, in_to_state, in_triggering_event)
	{
	}


	bool PlayingToPauseTransition::OnEnterImpl(chaos::SM::StateMachineInstance * sm_instance, chaos::SM::StateBase * from, chaos::ReferencedObject * extra_data)
	{
		Game * game = GetGame(sm_instance);
		if (game == nullptr)
			return true;
		game->OnEnterPause();
		return false;
	}

	bool PlayingToPauseTransition::TickImpl(chaos::SM::StateMachineInstance * sm_instance, double delta_time, chaos::ReferencedObject * extra_data)
	{
		Game * game = GetGame(sm_instance);
		if (game == nullptr)
			return true;
		return game->IsPauseEnterComplete();
	}

	PauseToPlayingTransition::PauseToPlayingTransition(GameState * in_from_state, GameState * in_to_state, chaos::TagType in_triggering_event) :
		GameTransition(in_from_state, in_to_state, in_triggering_event)
	{
	}

	bool PauseToPlayingTransition::OnEnterImpl(chaos::SM::StateMachineInstance * sm_instance, chaos::SM::StateBase * from, chaos::ReferencedObject * extra_data)
	{
		Game * game = GetGame(sm_instance);
		if (game == nullptr)
			return true;
		game->OnLeavePause();
		return false;
	}

	bool PauseToPlayingTransition::TickImpl(chaos::SM::StateMachineInstance * sm_instance, double delta_time, chaos::ReferencedObject * extra_data)
	{
		Game * game = GetGame(sm_instance);
		if (game == nullptr)
			return true;
		return game->IsPauseLeaveComplete();
	}

	PlayingToGameOverTransition::PlayingToGameOverTransition(GameState * in_from_state, GameState * in_to_state, chaos::TagType in_triggering_event) :
		GameTransition(in_from_state, in_to_state, in_triggering_event)
	{
	}

	bool PlayingToGameOverTransition::OnEnterImpl(chaos::SM::StateMachineInstance * sm_instance, chaos::SM::StateBase * from, chaos::ReferencedObject * extra_data)
	{
		Game * game = GetGame(sm_instance);
		if (game != nullptr)
		{
			sm_instance->SetContextData(game->PlaySound("gameover", false, false));
			game->OnGameOver();
		}
		return false;
	}

	bool PlayingToGameOverTransition::OnLeaveImpl(chaos::SM::StateMachineInstance * sm_instance, chaos::SM::StateBase * to, chaos::ReferencedObject * extra_data)
	{
		// destroy the sound object
		sm_instance->SetContextData(nullptr);
		// notify the game that it is finished
		Game * game = GetGame(sm_instance);
		if (game != nullptr)
			game->OnLeaveGame();
		return true;
	}

	bool PlayingToGameOverTransition::TickImpl(chaos::SM::StateMachineInstance * sm_instance, double delta_time, chaos::ReferencedObject * extra_data)
	{
		// wait until game over sound is finished
		chaos::Sound * gameover_sound = dynamic_cast<chaos::Sound*>(sm_instance->GetContextData());
		if (gameover_sound != nullptr)
			if (!gameover_sound->IsFinished())
				return false;
		// wait until the music is blend correctly
		Game * game = GetGame(sm_instance);
		if (game != nullptr)
			return game->IsGameLeaveComplete();
		return true;
	}

	// =========================================================
	// GameStateMachine
	// =========================================================

	GameStateMachine::GameStateMachine(class Game * in_game) :
		game(in_game)
	{
		assert(in_game != nullptr);
	}

	bool GameStateMachine::InitializeStateMachine()
	{
		main_menu_state = new MainMenuState(this);
		playing_state = new PlayingState(this);
		pause_state = new PauseState(this);

		main_menu_to_playing = new MainMenuToPlayingTransition(main_menu_state.get(), playing_state.get(), GameStateMachineKeys::EVENT_START_GAME);
		playing_to_main_menu = new PlayingToMainMenuTransition(playing_state.get(), main_menu_state.get(), GameStateMachineKeys::EVENT_EXIT_GAME);
		playing_to_pause = new PlayingToPauseTransition(playing_state.get(), pause_state.get(), GameStateMachineKeys::EVENT_TOGGLE_PAUSE);
		pause_to_playing = new PauseToPlayingTransition(pause_state.get(), playing_state.get(), GameStateMachineKeys::EVENT_TOGGLE_PAUSE);
		playing_to_gameover = new PlayingToGameOverTransition(playing_state.get(), main_menu_state.get(), GameStateMachineKeys::EVENT_GAME_OVER);

		SetInitialState(main_menu_state.get());

		return true;
	}

	// =========================================================
	// GameStateMachineInstance
	// =========================================================

	GameStateMachineInstance::GameStateMachineInstance(Game * in_game, chaos::SM::StateMachine * in_state_machine) : 
		chaos::SM::StateMachineInstance(in_state_machine),
		game(in_game)
	{
		assert(in_game != nullptr);
	}

}; // namespace death

