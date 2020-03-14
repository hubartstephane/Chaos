#include "Ludum45Level.h"
#include "Ludum45LevelInstance.h"
#include "Ludum45Game.h"
#include "Ludum45Player.h"
#include "Ludum45GameInstance.h"

#include <chaos/GLMTools.h>
#include <chaos/ParticleDefault.h>
#include <chaos/GeometryFramework.h>

// =============================================================
// LudumLevelInstance implementation
// =============================================================

LudumLevelInstance::LudumLevelInstance(LudumGame * in_game):
	game(in_game)
{
	assert(in_game != nullptr); 
}

void LudumLevelInstance::CreateCameras()
{
	death::TiledMapLevelInstance::CreateCameras();

	size_t camera_count = cameras.size();
	for (size_t i = 0; i < camera_count; ++i)
	{
		//cameras[i]->SetSafeZone(glm::vec2(0.6f, 0.8f));

		cameras[i]->SetSafeZone(glm::vec2(0.9f, 0.6f));


		//cameras[i]->AddComponent(new death::FollowPlayerCameraComponent(0));
		cameras[i]->AddComponent(new death::ShakeCameraComponent(0.15f, 0.05f, 0.15f, true, true));
		cameras[i]->AddComponent(new death::SoundListenerCameraComponent());
	}
}

bool LudumLevelInstance::DoTick(float delta_time)
{
	death::TiledMapLevelInstance::DoTick(delta_time);

	// get the game
	LudumGame * ludum_game = GetGame();
	if (ludum_game == nullptr)
		return true;
	// get the camera
	death::Camera* camera = GetCamera(0);
	if (camera == nullptr)
		return true;
	// get the camera boc without effects
	chaos::box2 camera_box = camera->GetCameraBox(false);
	if (IsGeometryEmpty(camera_box))
		return true;

	// move all players
	size_t player_count = GetPlayerCount();
	for (size_t i = 0; i < player_count; ++i)
	{
		// get the PLAYER 
		LudumPlayer* player = GetPlayer(i);
		if (player == nullptr)
			continue;
		// get the player box
		chaos::box2 player_box = player->GetPlayerBox();
		if (IsGeometryEmpty(player_box))
			continue;

	






		// compute scroll for both camera and player
		float scroll_displacement = ludum_game->scroll_factor * camera_speed * delta_time;
		float camera_x = camera_box.position.x + scroll_displacement; // the final wanted camera Y

		// the camera follows the player in X & Y direction
		chaos::box2 safe_camera = camera_box;
		safe_camera.half_size *= camera->GetSafeZone();
		chaos::RestrictToInside(safe_camera, player_box, true);
		camera_box.position = safe_camera.position;

		// force the camera Y position and player is now forced to be in Y camera range
		camera_box.position.x = camera_x;
		player_box.position.x += scroll_displacement;

		// force the player Y to be in camera box (X will not change because already in correct range from previous restrictionp)
		safe_camera = camera_box;
		safe_camera.half_size *= camera->GetSafeZone();
		chaos::RestrictToInside(safe_camera, player_box, false);

		// restrict camera to world
		chaos::box2 world = GetBoundingBox();
		if (!IsGeometryEmpty(world))
		{
			chaos::RestrictToInside(world, camera_box, false);
			chaos::RestrictToInside(world, player_box, false);
		}
		player->SetPlayerBox(player_box);

		// apply the compute result
		camera->SetCameraBox(camera_box);

	}
	

	return true;
}


bool LudumLevelInstance::Initialize(death::Game * in_game, death::Level * in_level)
{
	if (!death::TiledMapLevelInstance::Initialize(in_game, in_level))
		return false;

	death::TiledMapLevel const * level = GetLevel();
	if (level != nullptr)
	{
		camera_speed = level->GetTiledMap()->FindPropertyFloat("CAMERA_SPEED", camera_speed);
	}

	return true;
}

void LudumLevelInstance::OnPlayerLeaved(death::Player * player)
{
	death::TiledMapLevelInstance::OnPlayerLeaved(player);

	LudumPlayer * ludum_player = auto_cast(player);
    if (ludum_player != nullptr)
        ludum_player->DoUpdateBrightSideOfLife(false);
}

death::LevelCheckpoint * LudumLevelInstance::DoCreateCheckpoint() const
{
	return new LudumLevelCheckpoint();
}

bool LudumLevelInstance::DoLoadFromCheckpoint(death::LevelCheckpoint const * checkpoint)
{
	LudumLevelCheckpoint const * ludum_checkpoint = auto_cast(checkpoint);
	if (ludum_checkpoint == nullptr)
		return false;


	// destroy all bullets and all enemies
	char const * layer_names[] = { "PlayerFire", "EnemyFire", "Enemies", "Bonus", nullptr };
	for (int i = 0; layer_names[i] != nullptr; ++i)
	{
		death::TiledMapLayerInstance * layer_instance = FindLayerInstance(layer_names[i]);
		if (layer_instance != nullptr)
		{
			chaos::ParticleLayerBase * particle_layer = layer_instance->GetParticleLayer();
			if (particle_layer != nullptr)
				particle_layer->ClearAllAllocations();
		}
	}

	if (!death::TiledMapLevelInstance::DoLoadFromCheckpoint(ludum_checkpoint))
		return false;

	// shuludum .... refactor the share this code ... hard copy of what is in LD44

	return true;
}

bool LudumLevelInstance::DoSaveIntoCheckpoint(death::LevelCheckpoint * checkpoint) const
{
	LudumLevelCheckpoint * ludum_checkpoint = auto_cast(checkpoint);
	if (ludum_checkpoint == nullptr)
		return false;

	if (!death::TiledMapLevelInstance::DoSaveIntoCheckpoint(ludum_checkpoint))
		return false;

	return true;
}


