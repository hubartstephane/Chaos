#include <death/GameInstance.h>
#include <death/Level.h>
#include <death/Player.h>
#include <death/Camera.h>
#include <death/GameCheckpoint.h>
#include <death/GameGettersImpl.h>




#include <chaos/WinTools.h>


namespace death
{
	DEATH_GAMEGETTERS_IMPLEMENT(GameInstance);

	int GameInstance::GetBestPlayerScore() const
	{
		int result = 0;

		size_t count = players.size();
		for (size_t i = 0; i < count; ++i)
			if (players[i]->GetScore() > result)
				result = players[i]->GetScore();
		return result;
	}

	bool GameInstance::OnGamepadInput(chaos::MyGLFW::PhysicalGamepad* in_physical_gamepad)
	{
		assert(in_physical_gamepad != nullptr);
		// try to give the gamepad to a player
		if (GivePhysicalGamepadToPlayer(in_physical_gamepad) != nullptr) 
			return true;
		// try to have another player enter the game (not in pause)
		if (!game->IsPaused())
			if (CreatePlayer(in_physical_gamepad) != nullptr)
				return true;

		return false;
	}

	bool GameInstance::OnKeyEventImpl(chaos::KeyEvent const& event)
	{
		size_t count = players.size();
		for (size_t i = 0; i < count; ++i)
			if (players[i]->OnKeyEvent(event))
				return true;
		return false;
	}

	bool GameInstance::OnCharEventImpl(unsigned int c)
	{
		size_t count = players.size();
		for (size_t i = 0; i < count; ++i)
			if (players[i]->OnCharEvent(c))
				return true;
		return false;
	}

	bool GameInstance::OnMouseButtonImpl(int button, int action, int modifier)
	{
		size_t count = players.size();
		for (size_t i = 0; i < count; ++i)
			if (players[i]->OnMouseButton(button, action, modifier))
				return true;
		return false;
	}

	bool GameInstance::OnMouseMoveImpl(double x, double y)
	{
		size_t count = players.size();
		for (size_t i = 0; i < count; ++i)
			if (players[i]->OnMouseMove(x, y))
				return true;
		return false;
	}

	Player * GameInstance::CreatePlayer(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad)
	{
		// ensure we can create a new player
		size_t count = players.size();
		if (count >= GetMaxPlayerCount())
			return nullptr;
		// create the new player
		Player * result = DoCreatePlayer();
		if (result == nullptr)
			return nullptr;
		// initialize the player
		if (!result->Initialize(this))
		{
			delete(result);
			return nullptr;
		}
		// insert the player in our list
		result->player_index = players.size();
		players.push_back(result);
		
		// give the physical device to the player
		result->CapturePhysicalGamepad(in_physical_gamepad);

		// let the player enter the game instance
		OnPlayerEntered(result);
		// let the player enter the level
		LevelInstance * level_instance = GetLevelInstance();
		if (level_instance != nullptr)
		{
			level_instance->OnPlayerEntered(result);
			result->OnLevelChanged(level_instance->GetLevel(), nullptr, level_instance);
		}

		return result;
	}

	Player * GameInstance::GivePhysicalGamepadToPlayer(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad)
	{
		size_t count = players.size();
		for (size_t i = 0; i < count; ++i)
			if (players[i]->CapturePhysicalGamepad(in_physical_gamepad))
				return players[i].get();
		return nullptr;

	}

	bool GameInstance::DoTick(float delta_time)
	{
		size_t count = players.size();
		for (size_t i = 0; i < count; ++i)
			players[i]->Tick(delta_time);
		return true;
	}

	size_t GameInstance::GetMaxPlayerCount() const
	{
		return 1;
	}

	Player * GameInstance::DoCreatePlayer()
	{
		return player_class.CreateInstance();
	}

	double GameInstance::GetMainClockTime() const
	{
		if (main_clock == nullptr)
			return 0.0;
		return main_clock->GetClockTime();
	}

	double GameInstance::GetGameClockTime() const
	{
		if (game_clock == nullptr)
			return 0.0;
		return game_clock->GetClockTime();
	}

	double GameInstance::GetPauseClockTime() const
	{
		if (pause_clock == nullptr)
			return 0.0;
		return pause_clock->GetClockTime();
	}

	void GameInstance::FillUniformProvider(chaos::GPUProgramProvider & main_uniform_provider)
	{
		// the clocks
		double main_time = GetMainClockTime();
		main_uniform_provider.AddVariableValue("main_time", main_time);
		double game_time = GetGameClockTime();
		main_uniform_provider.AddVariableValue("game_time", game_time);
		double pause_time = GetPauseClockTime();
		main_uniform_provider.AddVariableValue("pause_time", pause_time);
		// the main player pawn box
		PlayerPawn const * player_pawn = GetPlayerPawn(0);
		if (player_pawn != nullptr)
		{
			chaos::box2 pawn_box = player_pawn->GetBox();
			main_uniform_provider.AddVariableValue("pawn_box", chaos::EncodeBoxToVector(pawn_box));
		}
	}

	bool GameInstance::Initialize(death::Game * in_game)
	{
		// ensure valid arguments and not already initialized
		assert(in_game != nullptr);
		assert(game == nullptr);

		game = in_game;

		// initialize from configuration
		if (!InitializeGameValues(in_game->game_instance_configuration, in_game->configuration_path, false)) // false for not hot reload
			return false;
		OnGameValuesChanged(false);

		// create the game instance clocks
		chaos::Clock * root_clock = in_game->GetRootClock();
		if (root_clock == nullptr)
			return false;

		// main  clock : reseted whenever a new game starts/ends. never paused
		if (main_clock == nullptr)
		{
			main_clock = root_clock->CreateChildClock("main_clock");
			if (main_clock == nullptr)
				return false;
		}
		// game clock : reseted whenever a new game starts/ends. paused in MainMenu and Pause
		if (game_clock == nullptr)
		{
			game_clock = root_clock->CreateChildClock("game_clock");
			if (game_clock == nullptr)
				return false;
		}
		// pause clock : reseted whenever we enter/leave pause. only running during pause
		if (pause_clock == nullptr)
		{
			chaos::ClockCreateParams pause_clock_params;
			pause_clock_params.paused = true;
			pause_clock = root_clock->CreateChildClock("pause_clock", pause_clock_params); // start paused ...
			if (pause_clock == nullptr)
				return false;
		}
		// create a sound category 
		chaos::SoundManager * sound_manager = game->GetSoundManager();
		if (sound_manager != nullptr)
			sound_category = sound_manager->AddCategory("game_instance");

		return true;
	}

	void GameInstance::OnEnterPause()
	{
		OnPauseStateUpdateClocks(true);
	}

	void GameInstance::OnLeavePause()
	{
		OnPauseStateUpdateClocks(false);
	}

	void GameInstance::OnGameOver()
	{
		if (game_clock != nullptr)
			game_clock->SetPause(true);
	}

	void GameInstance::OnPauseStateUpdateClocks(bool enter_pause)
	{
		// pause/resume the clocks
		if (pause_clock != nullptr)
		{
			pause_clock->SetPause(!enter_pause);
			pause_clock->Reset();
		}

		if (game_clock != nullptr)
			game_clock->SetPause(enter_pause);

		LevelInstance * level_instance = GetLevelInstance();
		if (level_instance != nullptr)
		{
			chaos::Clock * level_clock = level_instance->GetLevelClock();
			if (level_clock != nullptr)
				level_clock->SetPause(enter_pause);		
		}
	}

	bool GameInstance::RespawnPlayer(Player* player)
	{

		// LD46 : infinite life (life_count < 0 at any moment)

		// enough life ?
		int life_count = player->GetLifeCount();
		if (life_count == 1) // player is loosing its last life
			return false;
		// keep some values to restore later
		int score = player->GetScore();
		// try to go to checkpoint
		if (!RestartFromRespawnCheckpoint())
			return false;
		// update player values after death
		player->SetScore(score, false);
		if (life_count > 0)
			player->SetLifeCount(life_count - 1, false); // life_count <= 0 : player has immortality
		player->OnLifeLost();
		return true;
	}

	bool GameInstance::DoCheckGameOverCondition()
	{
		bool any_player_alive = false;

		death::LevelInstance* level_instance = GetLevelInstance();

		// compute whether all player are dead
		size_t player_count = GetPlayerCount();
		for (size_t i = 0; i < player_count; ++i)
		{
			death::Player* player = GetPlayer(i);
			if (player == nullptr)
				continue;

			// check whether the player is dead (from player point of view or level instance)
			bool player_dead = player->IsDead();
			if (!player_dead && level_instance != nullptr)
				player_dead = level_instance->IsPlayerDead(player);

			// player still alive
			if (!player_dead) 
			{
				any_player_alive = true;
			}
			// player dead
			else
			{
				if (RespawnPlayer(player))
					any_player_alive = true;
			}
		}

		// no more living player, end the game
		if (!any_player_alive)
			return true;
		return false;
	}

	void GameInstance::OnLevelChanged(Level * new_level, Level * old_level, LevelInstance * new_level_instance)
	{
		size_t count = GetPlayerCount();
		for (size_t i = 0; i < count; ++i)
		{
			Player* player = GetPlayer(i);
			if (player == nullptr)
				continue;
			player->OnLevelChanged(new_level, old_level, new_level_instance);
		}
	}
	
	void GameInstance::OnPlayerEntered(Player * player)
	{
		assert(player != nullptr);
	}
	
	void GameInstance::OnPlayerLeaved(Player * player)
	{
		assert(player != nullptr);
	}

	bool GameInstance::CreateRespawnCheckpoint()
	{
		respawn_checkpoint = SaveIntoCheckpoint();
		return (respawn_checkpoint != nullptr);
	}

	bool GameInstance::RestartFromRespawnCheckpoint()
	{
		if (respawn_checkpoint == nullptr)
			return false;		
		return LoadFromCheckpoint(respawn_checkpoint.get());
	}

	bool GameInstance::DoSaveIntoCheckpoint(GameCheckpoint * checkpoint) const
	{
		// save level instance data
		// XXX : this is important that it is first so we can test LEVEL INDEX correspond to the level
		LevelInstance const* level_instance = GetLevelInstance();
		if (level_instance != nullptr)
			if (!SaveIntoJSON(checkpoint->level_save, level_instance))
				return false;

		// save player data
		Player const* player = GetPlayer(0);
		if (player != nullptr)
			if (!SaveIntoJSON(checkpoint->player_save, player))
				return false;

		// save the clocks
#if 0
		if (main_clock != nullptr)
			if (!SaveIntoJSON(checkpoint->main_clock_save, main_clock))
				return false;
		if (main_clock != nullptr)
			if (!SaveIntoJSON(checkpoint->game_clock_save, game_clock))
				return false;
#endif

		

		auto p = checkpoint->level_save.dump(2);
		chaos::WinTools::CopyStringToClipboard(p.c_str());

		auto l = checkpoint->player_save.dump(2);
		chaos::WinTools::CopyStringToClipboard(l.c_str());


#if 0


		// save level data
		LevelInstance const * level_instance = GetLevelInstance();
		if (level_instance != nullptr)
		{
			checkpoint->level_index      = game->GetLevel()->GetLevelIndex();
			checkpoint->level_checkpoint = level_instance->SaveIntoCheckpoint();
		}

		// save player
		Player const * player = GetPlayer(0);
		if (player != nullptr)
			checkpoint->player_checkpoint = player->SaveIntoCheckpoint();

		// save the clocks
		if (main_clock != nullptr)
			checkpoint->main_clock_time = main_clock->GetClockTime();
		if (game_clock != nullptr)
			checkpoint->game_clock_time = game_clock->GetClockTime();

#endif


		return true;
	}

	bool GameInstance::DoLoadFromCheckpoint(GameCheckpoint const * checkpoint)
	{
		// load level instance data
		// XXX : this is important that it is first so we can test LEVEL INDEX correspond to the level
		LevelInstance * level_instance = GetLevelInstance();
		if (level_instance != nullptr)
			if (!LoadFromJSON(checkpoint->level_save, *level_instance))
				return false;

		// load player data
		Player * player = GetPlayer(0);
		if (player != nullptr)
			if (!LoadFromJSON(checkpoint->player_save, *player))
				return false;















#if 0
		// ensure the level is the good one : or load new level
		if (game->GetLevel()->GetLevelIndex() != checkpoint->level_index)
		{


			return false;
		}


		// load level
		LevelInstance * level_instance = GetLevelInstance();
		if (level_instance != nullptr)
			 level_instance->LoadFromCheckpoint(checkpoint->level_checkpoint.get());

		// load player
		Player * player = GetPlayer(0);
		if (player != nullptr)
			player->LoadFromCheckpoint(checkpoint->player_checkpoint.get());

		// load the clocks
		//if (main_clock != nullptr)
		//	main_clock->SetClockTime(checkpoint->main_clock_time);
		//if (game_clock != nullptr)
		//	game_clock->SetClockTime(checkpoint->game_clock_time);
#endif

		return true;
	}

	bool GameInstance::CanCompleteLevel() const
	{
		return true;
	}

	void GameInstance::OnEnterGame()
	{

	}

	void GameInstance::OnLeaveGame()
	{
		// notify all players start the game instance
		size_t count = players.size();
		for (size_t i = 0; i < count; ++i)
			if (players[i] != nullptr)
				OnPlayerLeaved(players[i].get());
		// destroy the sound category
		if (sound_category != nullptr)
		{
			sound_category->Stop();
			sound_category = nullptr;
		}
	}

	chaos::SoundCategory * GameInstance::GetSoundCategory()
	{
		return sound_category.get();
	}

	chaos::SoundCategory const * GameInstance::GetSoundCategory() const
	{
		return sound_category.get();
	}

	bool GameInstance::InitializeGameValues(nlohmann::json const& config, boost::filesystem::path const& config_path, bool hot_reload)
	{
		// capture the player configuration
		nlohmann::json const* p_config = chaos::JSONTools::GetStructure(config, "player");
		if (p_config != nullptr && p_config->is_object())
			player_configuration = *p_config;
		else
			player_configuration = nlohmann::json();

		return true;
	}

	void GameInstance::OnGameValuesChanged(bool hot_reload)
	{
		size_t count = players.size();
		for (size_t i = 0; i < count; ++i)
		{
			Player* player = players[i].get();
			if (player == nullptr)
				continue;
			if (player->InitializeGameValues(player_configuration, game->configuration_path, hot_reload))
				player->OnGameValuesChanged(hot_reload);
		}
	}
	
	GLFWwindow* GameInstance::GetGLFWWindow() const
	{
		Game const * game = GetGame();
		if (game == nullptr)
			return nullptr;
		return game->GetGLFWWindow();
	}

}; // namespace death
