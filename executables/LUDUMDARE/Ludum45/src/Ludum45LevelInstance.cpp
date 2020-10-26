#include "Ludum45Level.h"
#include "Ludum45LevelInstance.h"
#include "Ludum45Game.h"
#include "Ludum45Player.h"
#include "Ludum45GameInstance.h"

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
		cameras[i]->SetSafeZone(glm::vec2(0.9f, 0.6f));

		cameras[i]->AddComponent(new death::ShakeCameraComponent(0.15f, 0.05f, 0.15f, true, true));
		cameras[i]->AddComponent(new death::SoundListenerCameraComponent());
		if (ludum_game != nullptr)
			cameras[i]->AddComponent(new death::ScrollCameraComponent(ludum_game->scroll_factor * camera_speed, chaos::Axis::AXIS_X));
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
				scroll_component->SetScrollSpeed(ludum_game->scroll_factor * camera_speed);
		}
	}
	return true;
}


bool LudumLevelInstance::Initialize(death::Game * in_game, death::Level * in_level)
{
	if (!death::TMLevelInstance::Initialize(in_game, in_level))
		return false;

	death::TMLevel const * level = GetLevel();
	if (level != nullptr)
	{
		camera_speed = level->GetTiledMap()->GetPropertyValueFloat("CAMERA_SPEED", camera_speed);
	}

	return true;
}

void LudumLevelInstance::OnPlayerLeaved(death::Player * player)
{
	LudumPlayer * ludum_player = auto_cast(player);
    if (ludum_player != nullptr)
        ludum_player->DoUpdateBrightSideOfLife(false);

	death::TMLevelInstance::OnPlayerLeaved(player);
}


