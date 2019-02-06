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

LudumLevelInstance::LudumLevelInstance(LudumGame * in_game):
	game(in_game)
{
	assert(in_game != nullptr); 
	camera_safe_zone = glm::vec2(0.2f, 0.2f);
}

DEATH_GAMEFRAMEWORK_IMPLEMENT_LEVELINSTANCE(Ludum);

bool LudumLevelInstance::IsLevelCompleted() const
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
	// update the timeout
	if (level_timeout > 0.0f && !game->GetCheatMode())
	{
		level_timeout -= (float)delta_time;
		if (level_timeout < 0.0f)
			level_timeout = 0.0f;
	}
	return true;
}

bool LudumLevelInstance::CheckGameOverCondition()
{
	if (level_timeout == 0.0f)
		return true;
	return false;
}

void LudumLevelInstance::OnLevelStarted()
{
	death::TiledMap::LevelInstance::OnLevelStarted();

	static float DEFAULT_LEVEL_TIMEOUT = 50.0f;
	static int   DEFAULT_LEVEL_PARTICLE_REQUIREMENT = 10;

	level_timeout = DEFAULT_LEVEL_TIMEOUT;
	level_particle_requirement = DEFAULT_LEVEL_PARTICLE_REQUIREMENT;

	// change the background image and the level time
	std::string const * background_name = nullptr;

	death::TiledMap::Level const * level = GetTiledLevel(); // dynamic_cast<death::TiledMap::Level const *>(new_level_instance->GetLevel());
	if (level != nullptr)
		background_name = level->GetTiledMap()->FindPropertyString("BACKGROUND_NAME");

	level_timeout = level->GetTiledMap()->FindPropertyFloat("LEVEL_TIME", DEFAULT_LEVEL_TIMEOUT);
	level_particle_requirement = level->GetTiledMap()->FindPropertyInt("LEVEL_PARTICLE_REQUIREMENT", DEFAULT_LEVEL_PARTICLE_REQUIREMENT);

	game->CreateBackgroundImage(nullptr, (background_name == nullptr) ? nullptr : background_name->c_str());
}
