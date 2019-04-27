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
	camera_safe_zone = glm::vec2(0.7f, 0.7f);
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
	// keep camera, player inside the world
	RestrictCameraToPlayerAndWorld(0);
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
