#include "chaos/Chaos.h"

#include "Ludum45Level.h"
#include "Ludum45LevelInstance.h"
#include "Ludum45Game.h"
#include "Ludum45Player.h"
#include "Ludum45GameInstance.h"

// =============================================================
// LudumLevelInstance implementation
// =============================================================

void LudumLevelInstance::CreateCameraComponents(chaos::Camera* camera, chaos::TMCameraTemplate* camera_template)
{
	chaos::TMLevelInstance::CreateCameraComponents(camera, camera_template);

	camera->SetSafeZone(glm::vec2(0.9f, 0.6f));
	camera->AddComponent(new chaos::ShakeCameraComponent(0.15f, 0.05f, 0.15f, true, true));
	camera->AddComponent(new chaos::SoundListenerCameraComponent());
	if (LudumGame* ludum_game = GetGame())
		camera->AddComponent(new chaos::ScrollCameraComponent(ludum_game->scroll_factor * camera_speed, chaos::Axis::AXIS_X));
}

bool LudumLevelInstance::DoTick(float delta_time)
{
	chaos::TMLevelInstance::DoTick(delta_time);

	LudumGame* ludum_game = GetGame();

	// get the camera
	if (ludum_game != nullptr)
	{
		chaos::Camera* camera = GetCamera(0);
		if (camera != nullptr)
		{
			chaos::ScrollCameraComponent* scroll_component = camera->FindComponentByClass<chaos::ScrollCameraComponent>();
			if (scroll_component != nullptr)
				scroll_component->SetScrollSpeed(ludum_game->scroll_factor * camera_speed);
		}
	}
	return true;
}


bool LudumLevelInstance::Initialize(chaos::Game * in_game, chaos::Level * in_level)
{
	if (!chaos::TMLevelInstance::Initialize(in_game, in_level))
		return false;

	chaos::TMLevel const * level = GetLevel();
	if (level != nullptr)
	{
		camera_speed = level->GetTiledMap()->GetPropertyValueFloat("CAMERA_SPEED", camera_speed);
	}

	return true;
}

void LudumLevelInstance::OnPlayerLeaved(chaos::Player * player)
{
	LudumPlayer * ludum_player = auto_cast(player);
    if (ludum_player != nullptr)
        ludum_player->DoUpdateBrightSideOfLife(false);

	chaos::TMLevelInstance::OnPlayerLeaved(player);
}


