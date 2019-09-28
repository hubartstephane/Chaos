#include <death/GameLevel.h>
#include <death/GameLevelInstance.h>
#include <death/Game.h>
#include <death/GameInstance.h>
#include <death/Player.h>
#include <death/GameCheckpoint.h>

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
		chaos::obox2 camera_obox = GetCameraOBox(0);
		main_uniform_provider.AddVariableValue("camera_transform", CameraTransform::GetCameraTransform(camera_obox));
		// convert OBOX into BOX
		chaos::box2 camera_box;
		camera_box.position  = camera_obox.position;
		camera_box.half_size = camera_obox.half_size;
		main_uniform_provider.AddVariableValue("camera_box", chaos::EncodeBoxToVector(camera_box)); 
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
		// destroy all sounds for our category
		if (sound_category != nullptr)
		{
			sound_category->Stop();
			sound_category = nullptr;
		}
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
			cameras[i]->SetInitialCameraOBox(cameras[i]->GetCameraOBox());
		// change background
		CreateBackgroundImage();
		// change music
		SetInGameMusic();
		// create respawn checkpoint
		if (player_count > 0)
			game_instance->CreateRespawnCheckpoint();
	}

	void GameLevelInstance::CreateBackgroundImage()
	{

	}

	void GameLevelInstance::SetInGameMusic()
	{
		game->SetInGameMusic("game_music");
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
		// create a sound category
		chaos::SoundManager * sound_manager = GetGame()->GetSoundManager();
		if (sound_manager != nullptr)
			sound_category = sound_manager->AddCategory("level_instance");

		return true;
	}

	bool GameLevelInstance::GetLevelCompletionFlag() const
	{
		return level_completion_flag;
	}

	void GameLevelInstance::SetLevelCompletionFlag()
	{
		level_completion_flag = true;
	}

	bool GameLevelInstance::CheckLevelCompletion() const
	{
		if (level_completion_flag)
			return true;
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
		chaos::box2 camera_box = camera->GetCameraBox(false);
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

	bool GameLevelInstance::CanTick()
	{
		if (game->IsFreeCameraMode()) // in Free camera mode, do not tick level any more
			return false;
		return true;
	}

	bool GameLevelInstance::DoTick(double delta_time)
	{	
		// update the current camera
		Camera * current_camera = GetCamera(0);
		if (current_camera != nullptr)
			current_camera->Tick(delta_time);

#if 0
		size_t count = cameras.size();
		for (size_t i = 0; i < count; ++i)
			cameras[i]->Tick(delta_time);
#endif

		// shuxxx



		// update the timeout
		if (level_timeout > 0.0f && !CheckLevelCompletion() 
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
		// try the free camera mode first
		if (game->IsFreeCameraMode())
		{
			Camera * result = game->GetFreeCamera();
			if (result != nullptr)
				return result;
		}
		// level instance camera
		return DoGetCamera(index);
	}

	Camera * GameLevelInstance::DoGetCamera(size_t index)
	{		
		if (index >= cameras.size())
			return nullptr;
		return cameras[index].get();
	}

	Camera const * GameLevelInstance::GetCamera(size_t index) const
	{
		// try the free camera mode first
		if (game->IsFreeCameraMode())
		{
			Camera const * result = game->GetFreeCamera();
			if (result != nullptr)
				return result;		
		}
		// level instance camera
		return DoGetCamera(index);
	}

	Camera const * GameLevelInstance::DoGetCamera(size_t index) const
	{
		if (index >= cameras.size())
			return nullptr;
		return cameras[index].get();
	}

	chaos::obox2 GameLevelInstance::GetInitialCameraOBox(size_t index) const
	{ 
		Camera const * camera = GetCamera(index);
		if (camera == nullptr)
			return chaos::obox2();
		return camera->GetInitialCameraOBox();
	}

	chaos::obox2 GameLevelInstance::GetCameraOBox(size_t index, bool apply_modifiers) const
	{
		// find a camera
		Camera const * camera = GetCamera(index);
		if (camera != nullptr)
			return camera->GetCameraOBox(apply_modifiers);
		// fallback code
		return GetDefaultCameraOBox();
	}

	chaos::obox2 GameLevelInstance::GetDefaultCameraOBox() const
	{
		chaos::obox2 result;
		result.position = glm::vec3(0.0f, 0.0f, 0.0f);
		result.half_size = GetGame()->GetCanvasSize() * 0.5f;
		result.rotator = 0.0f;
		return result;
	}

	chaos::box2 GameLevelInstance::GetCameraBox(size_t index, bool apply_modifiers) const 
	{ 
		Camera const * camera = GetCamera(index);
		if (camera == nullptr)
			return chaos::box2();
		return camera->GetCameraBox(apply_modifiers);
	}

	void GameLevelInstance::SetCameraBox(size_t index, chaos::box2 in_box)
	{ 
		Camera * camera = GetCamera(index);
		if (camera == nullptr)
			return;
		camera->SetCameraBox(in_box);
	}




	bool GameLevelInstance::DoSaveIntoCheckpoint(LevelCheckpoint * checkpoint) const
	{
		checkpoint->camera_box    = GetCameraBox(0, false);
		checkpoint->level_timeout = level_timeout;

		//if (level_clock != nullptr)
		//	checkpoint->level_clock_time = level_clock->GetClockTime();

		return true;
	}

	bool GameLevelInstance::DoLoadFromCheckpoint(LevelCheckpoint const * checkpoint)
	{
		SetCameraBox(0, checkpoint->camera_box);
		level_timeout = checkpoint->level_timeout;

		//if (level_clock != nullptr)
		//	level_clock->SetClockTime(checkpoint->level_clock_time);

		return true;
	}

}; // namespace death

