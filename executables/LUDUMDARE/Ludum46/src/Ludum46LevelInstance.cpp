#include "Ludum46Level.h"
#include "Ludum46LevelInstance.h"
#include "Ludum46Game.h"
#include "Ludum46Player.h"
#include "Ludum46GameInstance.h"
#include "Ludum46PlayerDisplacementComponent.h"

#include <chaos/GLMTools.h>
#include <chaos/ParticleDefault.h>
#include <chaos/GeometryFramework.h>

#include <death/FollowPlayerCameraComponent.h>
#include <death/ShakeCameraComponent.h>
#include <death/SoundListenerCameraComponent.h>

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
		cameras[i]->SetSafeZone(glm::vec2(0.6f, 0.6f));
		cameras[i]->AddComponent(new death::FollowPlayerCameraComponent(0));
		cameras[i]->AddComponent(new death::ShakeCameraComponent(0.15f, 0.05f, 0.15f, true, true));
		cameras[i]->AddComponent(new death::SoundListenerCameraComponent());
	}
}

bool LudumLevelInstance::DoTick(float delta_time)
{
	death::TiledMapLevelInstance::DoTick(delta_time);


	return true;
}


void LudumLevelInstance::OnLevelStarted()
{
	// super call
	death::TiledMapLevelInstance::OnLevelStarted();
}

bool LudumLevelInstance::Initialize(death::Game * in_game, death::Level * in_level)
{
	if (!death::TiledMapLevelInstance::Initialize(in_game, in_level))
		return false;



	return true;
}

void LudumLevelInstance::OnPlayerEntered(death::Player * player)
{
	death::TiledMapLevelInstance::OnPlayerEntered(player);


}

void LudumLevelInstance::OnPlayerLeaved(death::Player * player)
{
	death::TiledMapLevelInstance::OnPlayerLeaved(player);


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


	if (!death::TiledMapLevelInstance::DoLoadFromCheckpoint(ludum_checkpoint))
		return false;





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

death::PlayerDisplacementComponent* LudumLevelInstance::CreatePlayerDisplacementComponent(death::Player* player)
{
	return new LudumPlayerDisplacementComponent(player);
}

