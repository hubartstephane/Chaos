#include "Ludum44Level.h"
#include "Ludum44LevelInstance.h"
#include "Ludum44Game.h"
#include "Ludum44Player.h"
#include "Ludum44GameInstance.h"

#include <death/ShakeCameraComponent.h>
#include <death/SoundListenerCameraComponent.h>
#include <death/ScrollCameraComponent.h>

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

	LudumGame* ludum_game = GetGame();

	size_t camera_count = cameras.size();
	for (size_t i = 0; i < camera_count; ++i)
	{
		cameras[i]->SetSafeZone(glm::vec2(0.6f, 0.8f));
		cameras[i]->AddComponent(new death::ShakeCameraComponent(0.15f, 0.05f, 0.15f, true, true));
		cameras[i]->AddComponent(new death::SoundListenerCameraComponent());
		if (ludum_game != nullptr)
			cameras[i]->AddComponent(new death::ScrollCameraComponent(ludum_game->scroll_factor * camera_speed, chaos::Axis::AXIS_Y));
	}
}

bool LudumLevelInstance::DoTick(float delta_time)
{
	death::TiledMapLevelInstance::DoTick(delta_time);

	LudumGame* ludum_game = GetGame();

	// get the camera
	if (ludum_game != nullptr)
	{
		death::Camera* camera = GetCamera(0);
		if (camera != nullptr)
		{
			death::ScrollCameraComponent* scroll_component = camera->FindComponentByClass<death::ScrollCameraComponent>();
			if (scroll_component != nullptr)
				scroll_component->SetScrollSpeed(ludum_game->scroll_factor * camera_speed);
		}
	}
	return true;
}

void LudumLevelInstance::OnLevelStarted()
{
	// create the fire particle layer if necessary
	death::TiledMapLayerInstance * layer_instance = FindLayerInstance("fire");
	if (layer_instance != nullptr)
		layer_instance->CreateParticleLayer();

	// super call
	death::TiledMapLevelInstance::OnLevelStarted();
}

bool LudumLevelInstance::Initialize(death::Game * in_game, death::Level * in_level)
{
	if (!death::TiledMapLevelInstance::Initialize(in_game, in_level))
		return false;

	// change the camera speed
	static float DEFAULT_CAMERA_SPEED = 100.0f;
	camera_speed = DEFAULT_CAMERA_SPEED;

	death::TiledMapLevel const * level = GetLevel();
	if (level != nullptr)
	{
		camera_speed = level->GetTiledMap()->FindPropertyFloat("CAMERA_SPEED", DEFAULT_CAMERA_SPEED);
	}

	return true;
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

	scroll_factor = ludum_checkpoint->scroll_factor;

	// destroy all bullets and all enemies
	char const * layer_names[] = { "fire", "Enemies", nullptr };
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

	return true;
}

bool LudumLevelInstance::DoSaveIntoCheckpoint(death::LevelCheckpoint * checkpoint) const
{
	LudumLevelCheckpoint * ludum_checkpoint = auto_cast(checkpoint);
	if (ludum_checkpoint == nullptr)
		return false;

	if (!death::TiledMapLevelInstance::DoSaveIntoCheckpoint(ludum_checkpoint))
		return false;

	ludum_checkpoint->scroll_factor = scroll_factor;


	return true;
}


