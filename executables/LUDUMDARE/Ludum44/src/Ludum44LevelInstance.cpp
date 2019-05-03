#include "Ludum44Level.h"
#include "Ludum44LevelInstance.h"
#include "Ludum44Game.h"
#include "Ludum44Player.h"
#include "Ludum44GameInstance.h"

#include <chaos/GLMTools.h>
#include <chaos/ParticleDefault.h>
#include <chaos/GeometryFramework.h>

// =============================================================
// LudumLevelInstance implementation
// =============================================================

DEATH_GAMEFRAMEWORK_IMPLEMENT_LEVELINSTANCE(Ludum);

LudumLevelInstance::LudumLevelInstance(LudumGame * in_game):
	game(in_game)
{
	assert(in_game != nullptr); 
}

glm::vec2 LudumLevelInstance::GetCameraSafeZone() const
{
	return glm::vec2(0.8f, 0.8f);
}

bool LudumLevelInstance::IsLevelCompleted(bool & loop_levels) const
{
	loop_levels = false;

	return level_completed;
}

bool LudumLevelInstance::CanCompleteLevel() const
{


	return true;
}

bool LudumLevelInstance::DoTick(double delta_time)
{
	death::TiledMap::LevelInstance::DoTick(delta_time);

	// get the game
	LudumGame * ludum_game = GetLudumGame();
	if (ludum_game == nullptr)
		return true;
	// get the PLAYER 0
	LudumPlayer * player = GetLudumPlayer(0);
	if (player == nullptr)
		return true;
	// restrict the player to the world
	RestrictPlayerToWorld(0);

	// get the camera BEFORE modification
	chaos::box2 camera_before = GetCameraBox();
	if (camera_before.IsEmpty())
		return true;
	// keep the player in camera view
	RestrictCameraToPlayerAndWorld(0);
	// get the camera AFTER modification
	chaos::box2 camera_after = GetCameraBox();
	if (camera_after.IsEmpty())
		return true;
	// correct camera position
	int scroll_direction = 1;

	float delta_camera = camera_before.position[scroll_direction] - camera_after.position[scroll_direction];

	chaos::box2 player_box = player->GetPlayerBox();

	if (delta_camera != 0.0f) // player forced a fast forward or a backward displacement : remove it
	{
		player_box.position[scroll_direction] += delta_camera;
		camera_after.position[scroll_direction] += delta_camera;
	}

	// correct camera and player position
	float scroll_displacement = scroll_factor * ludum_game->scroll_factor * camera_speed * (float)delta_time;

	camera_after.position[scroll_direction] += scroll_displacement;
	SetCameraBox(camera_after);

	player_box.position[scroll_direction] += scroll_displacement;
	player->SetPlayerBox(player_box);



	return true;
}

void LudumLevelInstance::OnLevelStarted()
{
	// change the background image
	std::string const * background_name = nullptr;

	death::TiledMap::Level const * level = GetTiledLevel();
	if (level != nullptr)
		background_name = level->GetTiledMap()->FindPropertyString("BACKGROUND_NAME");

	game->CreateBackgroundImage(nullptr, (background_name == nullptr) ? nullptr : background_name->c_str());

	// create the fire particle layer if necessary
	death::TiledMap::LayerInstance * layer_instance = FindLayerInstance("fire");
	if (layer_instance != nullptr)
		layer_instance->CreateParticleLayer();

	// super call
	death::TiledMap::LevelInstance::OnLevelStarted();
}

bool LudumLevelInstance::Initialize(death::Game * in_game, death::GameLevel * in_level)
{
	if (!death::TiledMap::LevelInstance::Initialize(in_game, in_level))
		return false;

	// change the level timeout + camera speed
	static float DEFAULT_LEVEL_TIMEOUT = -1.0f; // no time out
	level_timeout = DEFAULT_LEVEL_TIMEOUT;

	static float DEFAULT_CAMERA_SPEED = 100.0f;
	camera_speed = DEFAULT_CAMERA_SPEED;

	death::TiledMap::Level const * level = GetTiledLevel();
	if (level != nullptr)
	{
		level_timeout = level->GetTiledMap()->FindPropertyFloat("LEVEL_TIME", DEFAULT_LEVEL_TIMEOUT);
		camera_speed = level->GetTiledMap()->FindPropertyFloat("CAMERA_SPEED", DEFAULT_CAMERA_SPEED);
	}

	return true;
}

void LudumLevelInstance::OnPlayerEntered(death::Player * player)
{
	death::TiledMap::LevelInstance::OnPlayerEntered(player);

	LudumPlayer * ludum_player = auto_cast(player);
	if (ludum_player == nullptr)
		return;

	death::TiledMap::LayerInstance * layer_instance = FindLayerInstance("fire");
	if (layer_instance == nullptr)
		return;

	chaos::ParticleLayerBase * fire_layer = layer_instance->GetParticleLayer();
	if (fire_layer != nullptr)
		ludum_player->fire_allocation = fire_layer->SpawnParticles(0);
}

void LudumLevelInstance::OnPlayerLeaved(death::Player * player)
{
	death::TiledMap::LevelInstance::OnPlayerLeaved(player);

	LudumPlayer * ludum_player = auto_cast(player);
	if (ludum_player == nullptr)
		return;
	ludum_player->fire_allocation = nullptr;
}

