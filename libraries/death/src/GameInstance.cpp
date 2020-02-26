#include <death/GameInstance.h>
#include <death/Level.h>
#include <death/Player.h>
#include <death/GameCheckpoint.h>

namespace death
{
	GameInstance::GameInstance(Game * in_game) :
		game(in_game)
	{
		assert(in_game != nullptr);
	}

	
	chaos::AutoCastable<Level> GameInstance::GetLevel()
	{
		return game->GetLevel();
	}
	
	chaos::AutoConstCastable<Level> GameInstance::GetLevel() const
	{
		return game->GetLevel();
	}

	chaos::AutoCastable<LevelInstance> GameInstance::GetLevelInstance()
	{
		return game->GetLevelInstance();
	}
	
	chaos::AutoConstCastable<LevelInstance> GameInstance::GetLevelInstance() const
	{
		return game->GetLevelInstance();
	}

	chaos::AutoCastable<Player> GameInstance::GetPlayer(size_t player_index)
	{
		// shuxxx check player_index is initialized (seems NOT)

		size_t count = players.size();
		for (size_t i = 0; i < count; ++i)
			if (players[i]->GetPlayerIndex() == player_index)
				return players[i].get();
		return nullptr;
	}

	chaos::AutoConstCastable<Player> GameInstance::GetPlayer(size_t player_index) const
	{
		// shuxxx check player_index is initialized (seems NOT)

		size_t count = players.size();
		for (size_t i = 0; i < count; ++i)
			if (players[i]->GetPlayerIndex() == player_index)
				return players[i].get();
		return nullptr;
	}

	int GameInstance::GetBestPlayerScore() const
	{
		int result = 0;

		size_t count = players.size();
		for (size_t i = 0; i < count; ++i)
			if (players[i]->GetScore() > result)
				result = players[i]->GetScore();
		return result;
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
			return result;
		}
		// insert the player in our list
		players.push_back(result);
		// give the physical device to the player
		result->CapturePhysicalGamepad(in_physical_gamepad);

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
		return new Player(this);
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
		// the main player box
		Player const * player = GetPlayer(0);
		if (player != nullptr)
		{
			chaos::box2 player_box = player->GetPlayerBox();
			main_uniform_provider.AddVariableValue("player_box", chaos::EncodeBoxToVector(player_box));
		}
	}

	bool GameInstance::Initialize(death::Game * in_game)
	{
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

	bool GameInstance::DoCheckGameOverCondition()
	{
		// player has no life any more
		Player * player = GetPlayer(0);
		if (player != nullptr)
			if (player->GetLifeCount() <= 0)
				return true;

		return false;
	}

	void GameInstance::OnLevelChanged(Level * new_level, Level * old_level, LevelInstance * new_level_instance)
	{

	}
	
	void GameInstance::OnPlayerEntered(Player * player)
	{

	}
	
	void GameInstance::OnPlayerLeaved(Player * player)
	{

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

		return true;
	}

	bool GameInstance::DoLoadFromCheckpoint(GameCheckpoint const * checkpoint)
	{
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
