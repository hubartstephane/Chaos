#include "Ludum43Level.h"
#include "Ludum43LevelInstance.h"
#include "Ludum43Game.h"
#include "Ludum43Player.h"
#include "Ludum43GameInstance.h"

#include <chaos/GLMTools.h>
#include <chaos/ParticleDefault.h>
#include <chaos/GeometryFramework.h>

#include <death/CameraComponent.h>

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
	for (size_t i = 0; i < camera_count; ++i)
	{
		cameras[i]->SetSafeZone(glm::vec2(0.2f, 0.2f));
		cameras[i]->AddComponent(new death::FollowPlayerCameraComponent(0));
		cameras[i]->AddComponent(new death::SoundListenerCameraComponent());
	}
}

bool LudumLevelInstance::CheckLevelCompletion() const
{
	if (death::TiledMap::LevelInstance::CheckLevelCompletion())
		return true;

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

bool LudumLevelInstance::Initialize(death::Game * in_game, death::Level * in_level)
{
	if (!death::TiledMap::LevelInstance::Initialize(in_game, in_level))
		return false;
	// change the level timeout
	level_timeout = in_level->GetLevelTimeout();
	return true;
}
