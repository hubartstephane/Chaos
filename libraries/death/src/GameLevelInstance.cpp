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

		CameraTransform camera_transform = GetCameraTransform(0);
		main_uniform_provider.AddVariableValue("camera_transform", camera_transform.transform);
		main_uniform_provider.AddVariableValue("view_half_size", camera_transform.view_half_size);



		// shuxxx shuwww

		/*
		chaos::box2 camera_box;
		camera_box.position = camera_transform.transform.
		*/
		chaos::box2 camera = GetCameraBox(0);
		if (IsGeometryEmpty(camera))
			camera = game->GetViewBox();



		main_uniform_provider.AddVariableValue("camera_box", chaos::EncodeBoxToVector(camera));
	}

	void GameLevelInstance::OnPlayerEntered(Player * player)
	{

	}

	void GameLevelInstance::OnPlayerLeaved(Player * player)
	{
	
	}

	void GameLevelInstance::DestroyCameras()
	{
		cameras.clear(); // destroy all camera
	}

	void GameLevelInstance::CreateCameras()
	{

	}

	void GameLevelInstance::OnLevelEnded()
	{
		GameInstance * game_instance = GetGameInstance();
		if (game_instance == nullptr)
			return;
		// players leaving the level
		size_t player_count = game_instance->GetPlayerCount();
		for (size_t i = 0; i < player_count; ++i)
			OnPlayerLeaved(game_instance->GetPlayer(i));
		// destroy cameras
		DestroyCameras();
	}
	
	void GameLevelInstance::OnLevelStarted()
	{
		GameInstance * game_instance = GetGameInstance();
		if (game_instance == nullptr)
			return;
		// players entering the level
		size_t player_count = game_instance->GetPlayerCount();
		for (size_t i = 0; i < player_count; ++i)
			OnPlayerEntered(game_instance->GetPlayer(i));
		// create cameras
		CreateCameras();
		// last initialization of camera
		size_t camera_count = cameras.size();
		for (size_t i = 0; i < camera_count; ++i)
			cameras[i]->SetInitialCameraBox(cameras[i]->GetCameraBox());
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

	bool GameLevelInstance::IsLevelCompleted(bool & loop_levels) const
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

	void GameLevelInstance::RestrictCameraToPlayerAndWorld(int player_index, size_t camera_index)
	{
		// get the wanted player
		Player * player = GetPlayer(player_index);
		if (player == nullptr)
			return;

		// get the camera
		Camera * camera = GetCamera(camera_index);
		if (camera == nullptr)
			return;

		// get camera, cannot continue if it is empty
		chaos::box2 camera_box = camera->GetCameraBox();
		if (IsGeometryEmpty(camera_box))
			return;

		// keep player inside camera safe zone
		chaos::box2 player_box = player->GetPlayerBox();
		if (!IsGeometryEmpty(player_box))
		{
			chaos::box2 safe_camera = camera_box;
			safe_camera.half_size *= camera->GetSafeZone();

			if (chaos::RestrictToInside(safe_camera, player_box, true)) // apply the safe_zone displacement to the real camera
				camera_box.position = safe_camera.position;
		}

		// try to keep the camera in the world
		chaos::box2 world = GetBoundingBox();
		if (!IsGeometryEmpty(world))
			chaos::RestrictToInside(world, camera_box, false);

		// apply camera changes
		camera->SetCameraBox(camera_box);
	}

	void GameLevelInstance::RestrictObjectToWorld(chaos::ParticleAllocationBase * allocation, size_t index)
	{
		if (allocation == nullptr)
			return;
		chaos::box2 world = GetBoundingBox();
		if (IsGeometryEmpty(world))
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
		bool loop_levels = false; // unused

		// update the cameras
		size_t count = cameras.size();
		for (size_t i = 0; i < count; ++i)
			cameras[i]->Tick(delta_time);

		// update the timeout
		if (level_timeout > 0.0f 
			&& !IsLevelCompleted(loop_levels) 
#if _DEBUG	
			&& !game->GetCheatMode()
#endif
		){
			level_timeout -= (float)delta_time;
			if (level_timeout < 0.0f)
				level_timeout = 0.0f;
		}
		return true;
	}

	size_t GameLevelInstance::GetCameraCount() const
	{
		return cameras.size();
	}

	Camera * GameLevelInstance::GetCamera(size_t index)
	{
		if (index >= cameras.size())
			return nullptr;
		return cameras[index].get();
	}

	Camera const * GameLevelInstance::GetCamera(size_t index) const
	{
		if (index >= cameras.size())
			return nullptr;
		return cameras[index].get();
	}

	CameraTransform GameLevelInstance::GetCameraTransform(size_t index) const
	{
		// find a camera
		Camera const * camera = GetCamera(index);
		if (camera != nullptr)
			return camera->GetCameraTransform();
		// fallback code
		return GetDefaultCameraTransform();
	}

	CameraTransform GameLevelInstance::GetDefaultCameraTransform() const
	{
		CameraTransform result;
		result.transform = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
		result.view_half_size = GetGame()->GetViewSize() * 0.5f;
		return result;
	}



	chaos::box2 GameLevelInstance::GetCameraBox(size_t index) const 
	{ 
		Camera const * camera = GetCamera(index);
		if (camera == nullptr)
			return chaos::box2();
		return camera->GetCameraBox();
	}

	void GameLevelInstance::SetCameraBox(size_t index, chaos::box2 in_box)
	{ 
		Camera * camera = GetCamera(index);
		if (camera == nullptr)
			return;
		camera->SetCameraBox(in_box);
	}

	chaos::box2 GameLevelInstance::GetInitialCameraBox(size_t index) const
	{ 
		Camera const * camera = GetCamera(index);
		if (camera == nullptr)
			return chaos::box2();
		return camera->GetInitialCameraBox();
	}

}; // namespace death

