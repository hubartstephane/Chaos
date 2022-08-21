#include "chaos/Chaos.h"

#include "Ludum50Level.h"
#include "Ludum50LevelInstance.h"
#include "Ludum50Game.h"
#include "Ludum50Player.h"
#include "Ludum50GameInstance.h"
#include "Ludum50PlayerDisplacementComponent.h"
#include "Ludum50Particles.h"

// =============================================================
// LudumLevelInstance implementation
// =============================================================

LudumLevelInstance::LudumLevelInstance()
{
	player_displacement_component_class = LudumPlayerDisplacementComponent::GetStaticClass();
}

void LudumLevelInstance::CreateCameraComponents(Camera* camera, TMCameraTemplate* camera_template)
{
	TMLevelInstance::CreateCameraComponents(camera, camera_template);

	camera->SetSafeZone(glm::vec2(0.0f, 0.0f));
	camera->AddComponent(new FollowPlayerCameraComponent(0));
	camera->AddComponent(new ShakeCameraComponent(0.15f, 0.05f, 0.15f, true, true));
	camera->AddComponent(new SoundListenerCameraComponent());
}

bool LudumLevelInstance::DoTick(float delta_time)
{
	TMLevelInstance::DoTick(delta_time);




	return true;
}

bool LudumLevelInstance::Initialize(Game * in_game, Level * in_level)
{
	if (!TMLevelInstance::Initialize(in_game, in_level))
		return false;



	return true;
}


bool LudumLevelInstance::IsPlayerDead(Player* player)
{
	if (TMLevelInstance::IsPlayerDead(player))
		return true;

	return false;
}


bool LudumLevelInstance::CheckLevelCompletion() const
{
	LudumLevel const * ludum_level = GetLevel();

	LudumPlayer const * ludum_player = GetPlayer(0);
	if (ludum_player != nullptr && ludum_level != nullptr)
	{

	}
	return false;
}

bool LudumLevelInstance::CanCompleteLevel() const
{
	if (completion_timer == 0.0f)
		return true;
	return false;
}

int LudumLevelInstance::DoDisplay(GPURenderer* renderer, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params)
{



	return TMLevelInstance::DoDisplay(renderer, uniform_provider, render_params);
}

uint64_t LudumLevelInstance::GetCollisionFlagByName(char const* name) const
{
	assert(name != nullptr);




	return TMLevelInstance::GetCollisionFlagByName(name);
}
