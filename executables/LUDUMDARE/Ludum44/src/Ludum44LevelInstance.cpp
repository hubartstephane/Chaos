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
	camera_safe_zone = glm::vec2(0.9f, 0.7f);
}

bool LudumLevelInstance::IsLevelCompleted() const
{


	return false;
}

bool LudumLevelInstance::CanCompleteLevel() const
{


	return false;
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
	float delta_camera_x = camera_before.position.x - camera_after.position.x;

	chaos::box2 player_box = player->GetPlayerBox();

	if (delta_camera_x != 0.0f) // player forced a fast forward or a backward displacement : remove it
	{
		player_box.position.x += delta_camera_x;
		camera_after.position.x += delta_camera_x;
	}

	// correct camera and player position
	float scroll_displacement = ludum_game->scroll_factor * camera_speed * (float)delta_time;

	camera_after.position.x += scroll_displacement;
	SetCameraBox(camera_after);

	player_box.position.x += scroll_displacement;
	player->SetPlayerBox(player_box);

	return true;
}

void LudumLevelInstance::OnLevelStarted()
{
	death::TiledMap::LevelInstance::OnLevelStarted();

	// change the background image
	std::string const * background_name = nullptr;

	death::TiledMap::Level const * level = GetTiledLevel();
	if (level != nullptr)
		background_name = level->GetTiledMap()->FindPropertyString("BACKGROUND_NAME");

	game->CreateBackgroundImage(nullptr, (background_name == nullptr) ? nullptr : background_name->c_str());
}

bool LudumLevelInstance::Initialize(death::Game * in_game, death::GameLevel * in_level)
{
	if (!death::TiledMap::LevelInstance::Initialize(in_game, in_level))
		return false;

	// change the level timeout + camera speed
	static float DEFAULT_LEVEL_TIMEOUT = 50.0f;
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
