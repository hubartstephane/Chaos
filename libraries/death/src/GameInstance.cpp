#include <death/GameInstance.h>
#include <death/GameLevel.h>
#include <death/Player.h>

namespace death
{
	GameInstance::GameInstance(Game * in_game) :
		game(in_game)
	{
		assert(in_game != nullptr);
	}

	
	GameLevel * GameInstance::GetLevel()
	{
		return game->GetLevel();
	}
	
	GameLevel const * GameInstance::GetLevel() const
	{
		return game->GetLevel();
	}

	GameLevelInstance * GameInstance::GetLevelInstance()
	{
		return game->GetLevelInstance();
	}
	
	GameLevelInstance const * GameInstance::GetLevelInstance() const
	{
		return game->GetLevelInstance();
	}








	Player * GameInstance::GetPlayer(int player_index)
	{

		// shuxxx check player_index is initialized (seems NOT)








		size_t count = players.size();
		for (size_t i = 0; i < count; ++i)
			if (players[i]->GetPlayerIndex() == player_index)
				return players[i].get();
		return nullptr;
	}

	Player const * GameInstance::GetPlayer(int player_index) const
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

	bool GameInstance::OnKeyEvent(int key, int action)
	{
		size_t count = players.size();
		for (size_t i = 0; i < count; ++i)
			if (players[i]->OnKeyEvent(key, action))
				return true;
		return false;
	}

	bool GameInstance::OnCharEvent(unsigned int c)
	{
		size_t count = players.size();
		for (size_t i = 0; i < count; ++i)
			if (players[i]->OnCharEvent(c))
				return true;
		return false;
	}

	bool GameInstance::OnMouseButton(int button, int action, int modifier)
	{
		size_t count = players.size();
		for (size_t i = 0; i < count; ++i)
			if (players[i]->OnMouseButton(button, action, modifier))
				return true;
		return false;
	}

	bool GameInstance::OnMouseMove(double x, double y)
	{
		size_t count = players.size();
		for (size_t i = 0; i < count; ++i)
			if (players[i]->OnMouseMove(x, y))
				return true;
		return false;
	}

	Player * GameInstance::CreatePlayer(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad)
	{
		// ensure we can cerate a new player
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

	bool GameInstance::DoTick(double delta_time)
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
		if (pause_clock != nullptr)
		{
			pause_clock->SetPause(!enter_pause);
			pause_clock->Reset();
		}
		if (game_clock != nullptr)
			game_clock->SetPause(enter_pause);
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

	void GameInstance::OnLevelChanged(GameLevel * new_level, GameLevel * old_level, GameLevelInstance * new_level_instance)
	{

	}
	
	void GameInstance::OnPlayerEntered(Player * player)
	{

	}
	
	void GameInstance::OnPlayerLeaved(Player * player)
	{

	}

	void GameInstance::SetCheckpointPosition(glm::vec2 const & in_checkpoint_position, GameLevelInstance * in_checkpoint_level_instance)
	{
		if (in_checkpoint_level_instance != nullptr)
		{
			checkpoint_position = in_checkpoint_position;
			checkpoint_level_instance = in_checkpoint_level_instance;
			checkpoint_camera = in_checkpoint_level_instance->GetCameraBox(0);		
		}
	}

	bool GameInstance::IsCheckpointValid() const
	{
		return checkpoint_level_instance.get() != nullptr; // checkpoint still valid ?	
	}
	
	bool GameInstance::RestartFromCheckpoint(Player * player)
	{
		// update player particle
		chaos::ParticleDefault::Particle * player_particle = player->GetPlayerParticle();
		if (player_particle != nullptr)
		{
			if (!IsGeometryEmpty(checkpoint_camera))
				player_particle->bounding_box.position = checkpoint_camera.position;
			else
				player_particle->bounding_box.position = checkpoint_position;
		}
			
		// update camera position
		if (!IsGeometryEmpty(checkpoint_camera))
			checkpoint_level_instance->SetCameraBox(0, checkpoint_camera);	

		return OnRestartedFromCheckpoint(player);			
	}

	bool GameInstance::OnRestartedFromCheckpoint(Player * player)
	{
		GameLevelInstance * level_instance = GetLevelInstance();
		if (level_instance != nullptr)
			level_instance->RestrictCameraToPlayerAndWorld(0, 0);
		return true;
	}

}; // namespace death
