#include <death/GameLevel.h>
#include <death/GameLevelInstance.h>
#include <death/Game.h>
#include <death/GameInstance.h>
#include <death/Player.h>

#include <chaos/CollisionFramework.h>

namespace death
{

	// =====================================
	// GameLevelInstance implementation
	// =====================================

	GameLevel * GameLevelInstance::GetLevel() 
	{ 
		return level; 
	}

	GameLevel const * GameLevelInstance::GetLevel() const 
	{ 
		return level; 
	}

	Game * GameLevelInstance::GetGame() 
	{ 
		return game; 
	}
	
	Game const * GameLevelInstance::GetGame() const 
	{ 
		return game; 
	}

	GameInstance * GameLevelInstance::GetGameInstance() 
	{ 
		return game->GetGameInstance(); 
	}

	Player * GameLevelInstance::GetPlayer(int player_index)
	{
		GameInstance * game_instance = GetGameInstance();
		if (game_instance == nullptr)
			return nullptr;
		return game_instance->GetPlayer(player_index);
	}

	Player const * GameLevelInstance::GetPlayer(int player_index) const
	{
		GameInstance const * game_instance = GetGameInstance();
		if (game_instance == nullptr)
			return nullptr;
		return game_instance->GetPlayer(player_index);
	}

	GameInstance const * GameLevelInstance::GetGameInstance() const 
	{ 
		return game->GetGameInstance(); 
	}

	double GameLevelInstance::GetLevelClockTime() const
	{
		if (level_clock == nullptr)
			return 0.0;
		return level_clock->GetClockTime();
	}

	void GameLevelInstance::FillUniformProvider(chaos::GPUProgramProvider & main_uniform_provider)
	{
		// the timers
		double level_time = GetLevelClockTime();
		main_uniform_provider.AddVariableValue("level_time", level_time);
		// the main camera
		chaos::box2 camera = GetCameraBox();
		if (camera.IsEmpty())
			camera = game->GetViewBox();
		main_uniform_provider.AddVariableValue("camera_box", chaos::EncodeBoxToVector(camera));
	}

	void GameLevelInstance::OnPlayerEntered(Player * player)
	{

	}

	void GameLevelInstance::OnPlayerLeaved(Player * player)
	{
	
	}

	void GameLevelInstance::OnLevelEnded()
	{
		GameInstance * game_instance = GetGameInstance();
		if (game_instance == nullptr)
			return;
		size_t count = game_instance->GetPlayerCount();
		for (size_t i = 0; i < count; ++i)
			OnPlayerLeaved(game_instance->GetPlayer(i));
	}
	
	void GameLevelInstance::OnLevelStarted()
	{
		GameInstance * game_instance = GetGameInstance();
		if (game_instance == nullptr)
			return;
		size_t count = game_instance->GetPlayerCount();
		for (size_t i = 0; i < count; ++i)
			OnPlayerEntered(game_instance->GetPlayer(i));
	}

	bool GameLevelInstance::Initialize(Game * in_game, GameLevel * in_level)
	{
		assert(in_game != nullptr);
		assert(in_level != nullptr);
		game  = in_game;
		level = in_level;		
		// create the level clock
		chaos::Clock * root_clock = in_game->GetRootClock();
		if (root_clock == nullptr)
			return false;
		// note : this can fails if the named clock already exists
		level_clock = root_clock->CreateChildClock("level_clock"); 
		if (level_clock == nullptr)
			return false;

		return true;
	}

	bool GameLevelInstance::IsLevelCompleted() const
	{
		return false;
	}

	bool GameLevelInstance::CanCompleteLevel() const
	{
		return true; // no delay to effectively complete the level
	}

	void GameLevelInstance::OnEnterPause()
	{

	}

	void GameLevelInstance::OnLeavePause()
	{

	}

	void GameLevelInstance::OnGameOver()
	{

	}

	void GameLevelInstance::RestrictCameraToPlayerAndWorld(int player_index)
	{
		// get the wanted player
		Player * player = GetPlayer(player_index);
		if (player == nullptr)
			return;

		// get camera, cannot continue if it is empty
		chaos::box2 camera = GetCameraBox();
		if (camera.IsEmpty())
			return;

		// keep player inside camera safe zone
		chaos::box2 player_box = player->GetPlayerBox();
		if (!player_box.IsEmpty())
		{
			chaos::box2 safe_camera = camera;
			safe_camera.half_size *= GetCameraSafeZone();

			if (chaos::RestrictToInside(safe_camera, player_box, true)) // apply the safe_zone displacement to the real camera
				camera.position = safe_camera.position;
		}

		// try to keep the camera in the world
		chaos::box2 world = GetBoundingBox();
		if (!world.IsEmpty())
			chaos::RestrictToInside(world, camera, false);

		// apply camera changes
		SetCameraBox(camera);
	}

	void GameLevelInstance::RestrictObjectToWorld(chaos::ParticleAllocationBase * allocation, size_t index)
	{
		if (allocation == nullptr)
			return;
		chaos::box2 world = GetBoundingBox();
		if (world.IsEmpty())
			return;
		chaos::box2 box = chaos::ParticleDefault::GetParticleBox(allocation, index);		
		chaos::RestrictToInside(world, box, false);
		chaos::ParticleDefault::SetParticleBox(allocation, index, box);
	}

	void GameLevelInstance::RestrictPlayerToWorld(int player_index)
	{
		RestrictPlayerToWorld(GetPlayer(player_index));
	}

	void GameLevelInstance::RestrictPlayerToWorld(Player * player)
	{
		if (player == nullptr)
			return;
		RestrictObjectToWorld(player->GetPlayerAllocation(), 0);
	}

	bool GameLevelInstance::DoCheckGameOverCondition()
	{
		if (level_timeout == 0.0f)
			return true;
		return false;
	}

	bool GameLevelInstance::DoTick(double delta_time)
	{
		// update the timeout
		if (level_timeout > 0.0f && !IsLevelCompleted() && !game->GetCheatMode())
		{
			level_timeout -= (float)delta_time;
			if (level_timeout < 0.0f)
				level_timeout = 0.0f;
		}
		return true;
	}


}; // namespace death

