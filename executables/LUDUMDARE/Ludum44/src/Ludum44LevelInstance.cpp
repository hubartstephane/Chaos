#include "Ludum44Level.h"
#include "Ludum44LevelInstance.h"
#include "Ludum44Game.h"
#include "Ludum44Player.h"
#include "Ludum44GameInstance.h"

#include <chaos/Chaos.h>

#include <death/ShakeCameraComponent.h>
#include <death/SoundListenerCameraComponent.h>
#include <death/ScrollCameraComponent.h>



// =============================================================
// LudumLevelInstance implementation
// =============================================================

void LudumLevelInstance::CreateCameras()
{
	death::TMLevelInstance::CreateCameras();

	LudumGame* ludum_game = GetGame();

	size_t camera_count = cameras.size();
	for (size_t i = 0; i < camera_count; ++i)
	{
		cameras[i]->SetSafeZone(glm::vec2(0.6f, 0.8f));
		cameras[i]->AddComponent(new death::ShakeCameraComponent(0.15f, 0.05f, 0.15f, true, true));
		cameras[i]->AddComponent(new death::SoundListenerCameraComponent());
		if (ludum_game != nullptr)
			cameras[i]->AddComponent(new death::ScrollCameraComponent(ludum_game->scroll_factor * scroll_factor * camera_speed, chaos::Axis::AXIS_Y));
	}
}

bool LudumLevelInstance::DoTick(float delta_time)
{
	death::TMLevelInstance::DoTick(delta_time);

	LudumGame* ludum_game = GetGame();

	// get the camera
	if (ludum_game != nullptr)
	{
		death::Camera* camera = GetCamera(0);
		if (camera != nullptr)
		{
			death::ScrollCameraComponent* scroll_component = camera->FindComponentByClass<death::ScrollCameraComponent>();
			if (scroll_component != nullptr)
				scroll_component->SetScrollSpeed(ludum_game->scroll_factor * scroll_factor * camera_speed);
		}
	}
	return true;
}

void LudumLevelInstance::OnLevelStarted()
{
	// create the fire particle layer if necessary
	death::TMLayerInstance * layer_instance = FindLayerInstance("fire");
	if (layer_instance != nullptr)
		layer_instance->CreateParticleLayer();

	// super call
	death::TMLevelInstance::OnLevelStarted();
}

bool LudumLevelInstance::Initialize(death::Game * in_game, death::Level * in_level)
{
	if (!death::TMLevelInstance::Initialize(in_game, in_level))
		return false;

	// change the camera speed
	static float DEFAULT_CAMERA_SPEED = 100.0f;
	camera_speed = DEFAULT_CAMERA_SPEED;

	death::TMLevel const * level = GetLevel();
	if (level != nullptr)
	{
		camera_speed = level->GetTiledMap()->GetPropertyValueFloat("CAMERA_SPEED", DEFAULT_CAMERA_SPEED);
	}

	return true;
}

bool LudumLevelInstance::SerializeFromJSON(nlohmann::json const& json)
{
	if (!death::TMLevelInstance::SerializeFromJSON(json))
		return false;
	chaos::JSONTools::GetAttribute(json, "SCROLL_FACTOR", scroll_factor);
	return true;
}

bool LudumLevelInstance::SerializeIntoJSON(nlohmann::json & json) const
{
	if (!death::TMLevelInstance::SerializeIntoJSON(json))
		return false;
	chaos::JSONTools::SetAttribute(json, "SCROLL_FACTOR", scroll_factor);
	return true;
}

void LudumLevelInstance::SetScrollFactor(float in_scroll_factor) 
{ 
	scroll_factor = in_scroll_factor; 
}

float LudumLevelInstance::GetScrollFactor() const 
{ 
	return scroll_factor; 
}
