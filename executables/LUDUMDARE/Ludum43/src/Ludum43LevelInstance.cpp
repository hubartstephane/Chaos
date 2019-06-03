#include "Ludum43Level.h"
#include "Ludum43LevelInstance.h"
#include "Ludum43Game.h"
#include "Ludum43Player.h"
#include "Ludum43GameInstance.h"

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

void LudumLevelInstance::CreateCameras()
{
	// create the cameras
	death::TiledMap::LevelInstance::CreateCameras();
	// tweak the cameras
	size_t camera_count = cameras.size();
	for (size_t i = 0 ; i < camera_count ; ++i)
		cameras[i]->SetSafeZone(glm::vec2(0.2f, 0.2f));

}

bool LudumLevelInstance::IsLevelCompleted(bool & loop_levels) const
{
	loop_levels = true;

	if (game != nullptr)
	{
		ParticlePlayer * player_particle = game->GetPlayerParticle(0);
		if (player_particle != nullptr)
		{
			if (player_particle->level_end_reached)
				return true;
		}
	}
	return false;
}

bool LudumLevelInstance::CanCompleteLevel() const
{
	if (game != nullptr)
	{
		ParticlePlayer * player_particle = game->GetPlayerParticle(0);
		if (player_particle != nullptr)
		{
			if (player_particle->level_end_reached && player_particle->level_end_timer <= 0.0f)
				return true;
		}
	}
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

	// change the level timeout
	static float DEFAULT_LEVEL_TIMEOUT = 50.0f;
	static int   DEFAULT_LEVEL_PARTICLE_REQUIREMENT = 10;

	level_timeout = DEFAULT_LEVEL_TIMEOUT;
	level_particle_requirement = DEFAULT_LEVEL_PARTICLE_REQUIREMENT;

	death::TiledMap::Level const * level = GetTiledLevel();
	if (level != nullptr)
	{
		level_timeout = level->GetTiledMap()->FindPropertyFloat("LEVEL_TIME", DEFAULT_LEVEL_TIMEOUT);
		level_particle_requirement = level->GetTiledMap()->FindPropertyInt("LEVEL_PARTICLE_REQUIREMENT", DEFAULT_LEVEL_PARTICLE_REQUIREMENT);
	}
	return true;
}
