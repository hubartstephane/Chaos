#include <death/Level.h>
#include <death/LevelInstance.h>
#include <death/Game.h>
#include <death/GameInstance.h>
#include <death/Player.h>
#include <death/GameCheckpoint.h>

#include <chaos/CollisionFramework.h>

namespace death
{

	// =====================================
	// LevelInstance implementation
	// =====================================

	chaos::AutoCastable<Level> LevelInstance::GetLevel()
	{ 
		return level; 
	}

	chaos::AutoConstCastable<Level> LevelInstance::GetLevel() const
	{ 
		return level; 
	}

	chaos::AutoCastable<Game> LevelInstance::GetGame()
	{ 
		return game; 
	}
	
	chaos::AutoConstCastable<Game> LevelInstance::GetGame() const
	{ 
		return game; 
	}

	chaos::AutoCastable<GameInstance> LevelInstance::GetGameInstance()
	{ 
		return game->GetGameInstance(); 
	}

	chaos::AutoConstCastable<GameInstance> LevelInstance::GetGameInstance() const
	{
		return game->GetGameInstance();
	}

	chaos::AutoCastable<Player> LevelInstance::GetPlayer(size_t player_index)
	{
		GameInstance * game_instance = GetGameInstance();
		if (game_instance == nullptr)
			return nullptr;
		return game_instance->GetPlayer(player_index);
	}

	chaos::AutoConstCastable<Player> LevelInstance::GetPlayer(size_t player_index) const
	{
		GameInstance const * game_instance = GetGameInstance();
		if (game_instance == nullptr)
			return nullptr;
		return game_instance->GetPlayer(player_index);
	}

	double LevelInstance::GetLevelClockTime() const
	{
		if (level_clock == nullptr)
			return 0.0;
		return level_clock->GetClockTime();
	}

	void LevelInstance::FillUniformProvider(chaos::GPUProgramProvider & main_uniform_provider)
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

	void LevelInstance::OnPlayerEntered(Player * player)
	{

	}

	void LevelInstance::OnPlayerLeaved(Player * player)
	{
	
	}

	void LevelInstance::DestroyCameras()
	{
		cameras.clear(); // destroy all camera
	}

	void LevelInstance::CreateCameras()
	{

	}

	void LevelInstance::OnLevelEnded()
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
	
	void LevelInstance::OnLevelStarted()
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

	void LevelInstance::CreateBackgroundImage()
	{

	}

	void LevelInstance::SetInGameMusic()
	{
		game->SetInGameMusic("game_music");
	}

	bool LevelInstance::Initialize(Game * in_game, Level * in_level)
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

	bool LevelInstance::GetLevelCompletionFlag() const
	{
		return level_completion_flag;
	}

	void LevelInstance::SetLevelCompletionFlag()
	{
		level_completion_flag = true;
	}

	bool LevelInstance::CheckLevelCompletion() const
	{
		if (level_completion_flag)
			return true;
		return false;
	}

	bool LevelInstance::CanCompleteLevel() const
	{
		return true; // no delay to effectively complete the level
	}

	void LevelInstance::OnEnterPause()
	{

	}

	void LevelInstance::OnLeavePause()
	{

	}

	void LevelInstance::OnGameOver()
	{

	}

	void LevelInstance::RestrictCameraToPlayerAndWorld(size_t player_index, size_t camera_index)
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

	void LevelInstance::RestrictObjectToWorld(chaos::ParticleAllocationBase * allocation, size_t index)
	{
		if (allocation == nullptr)
			return;
		chaos::box2 world = GetBoundingBox();
		if (IsGeometryEmpty(world))
			return;
		chaos::box2 box = chaos::ParticleTools::GetParticleBox(allocation, index);		
		chaos::RestrictToInside(world, box, false);
		chaos::ParticleTools::SetParticleBox(allocation, index, box);
	}

	void LevelInstance::RestrictPlayerToWorld(Player * player)
	{
		if (player == nullptr)
			return;
		RestrictObjectToWorld(player->GetPlayerAllocation(), 0);
	}

	bool LevelInstance::DoCheckGameOverCondition()
	{
		if (level_timeout == 0.0f)
			return true;
		return false;
	}

	bool LevelInstance::CanTick()
	{
		if (game->IsFreeCameraMode()) // in Free camera mode, do not tick level any more
			return false;
		return true;
	}

	bool LevelInstance::DoTick(float delta_time)
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
			level_timeout -= delta_time;
			if (level_timeout < 0.0f)
				level_timeout = 0.0f;
		}	
		return true;
	}

	size_t LevelInstance::GetCameraCount() const
	{
		return cameras.size();
	}

	chaos::AutoCastable<Camera> LevelInstance::GetCamera(size_t index)
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

	Camera * LevelInstance::DoGetCamera(size_t index)
	{		
		if (index >= cameras.size())
			return nullptr;
		return cameras[index].get();
	}

	chaos::AutoConstCastable<Camera> LevelInstance::GetCamera(size_t index) const
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

	Camera const * LevelInstance::DoGetCamera(size_t index) const
	{
		if (index >= cameras.size())
			return nullptr;
		return cameras[index].get();
	}

	chaos::obox2 LevelInstance::GetInitialCameraOBox(size_t index) const
	{ 
		Camera const * camera = GetCamera(index);
		if (camera == nullptr)
			return chaos::obox2();
		return camera->GetInitialCameraOBox();
	}

	chaos::obox2 LevelInstance::GetCameraOBox(size_t index, bool apply_modifiers) const
	{
		// find a camera
		Camera const * camera = GetCamera(index);
		if (camera != nullptr)
			return camera->GetCameraOBox(apply_modifiers);
		// fallback code
		return GetDefaultCameraOBox();
	}

	chaos::obox2 LevelInstance::GetDefaultCameraOBox() const
	{
		// by default, the camera box has same size than the canvas
		chaos::obox2 result;
		result.position = glm::vec3(0.0f, 0.0f, 0.0f);
		result.half_size = GetGame()->GetCanvasBox().half_size;
		result.rotator = 0.0f;

		// apply the aspect ratio
		if (game != nullptr)
			chaos::AlterBoxToAspect(result, game->GetViewportWantedAspect(), true);

		return result;
	}

	chaos::box2 LevelInstance::GetCameraBox(size_t index, bool apply_modifiers) const 
	{ 
		Camera const * camera = GetCamera(index);
		if (camera == nullptr)
			return chaos::box2();
		return camera->GetCameraBox(apply_modifiers);
	}

	void LevelInstance::SetCameraBox(size_t index, chaos::box2 in_box)
	{ 
		Camera * camera = GetCamera(index);
		if (camera == nullptr)
			return;
		camera->SetCameraBox(in_box);
	}

	chaos::SoundCategory * LevelInstance::GetSoundCategory()
	{
		return sound_category.get();
	}

	chaos::SoundCategory const * LevelInstance::GetSoundCategory() const
	{
		return sound_category.get();
	}

	bool LevelInstance::DoSaveIntoCheckpoint(LevelCheckpoint * checkpoint) const
	{
		checkpoint->camera_box    = GetCameraBox(0, false);
		checkpoint->level_timeout = level_timeout;

		//if (level_clock != nullptr)
		//	checkpoint->level_clock_time = level_clock->GetClockTime();

		return true;
	}

	bool LevelInstance::DoLoadFromCheckpoint(LevelCheckpoint const * checkpoint)
	{
		SetCameraBox(0, checkpoint->camera_box);
		level_timeout = checkpoint->level_timeout;

		//if (level_clock != nullptr)
		//	level_clock->SetClockTime(checkpoint->level_clock_time);

		return true;
	}

	chaos::box2 LevelInstance::GetBoundingBox() const
	{
		if (game != nullptr)
			return game->GetCanvasBox();
		return chaos::box2();
	}

}; // namespace death

