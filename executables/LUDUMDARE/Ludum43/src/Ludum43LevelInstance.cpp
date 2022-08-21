#include "chaos/Chaos.h"

#include "Ludum43Level.h"
#include "Ludum43LevelInstance.h"
#include "Ludum43Game.h"
#include "Ludum43Player.h"
#include "Ludum43GameInstance.h"

// =============================================================
// LudumLevelInstance implementation
// =============================================================

void LudumLevelInstance::CreateCameraComponents(chaos::Camera* camera, chaos::TMCameraTemplate* camera_template)
{
	chaos::TMLevelInstance::CreateCameraComponents(camera, camera_template);

	camera->SetSafeZone(glm::vec2(0.2f, 0.2f));
	camera->AddComponent(new chaos::FollowPlayerCameraComponent(0));
	camera->AddComponent(new chaos::SoundListenerCameraComponent());
}

bool LudumLevelInstance::CheckLevelCompletion() const
{
	if (chaos::TMLevelInstance::CheckLevelCompletion())
		return true;

	LudumPlayer const * ludum_player = GetPlayer(0);
	if (ludum_player != nullptr)
	{
		ParticlePlayer const* player_particle = ludum_player->GetPlayerParticle();
		if (player_particle != nullptr)
			if (player_particle->level_end_reached)
				return true;
	}
	return false;
}

bool LudumLevelInstance::CanCompleteLevel() const
{
	LudumPlayer const* ludum_player = GetPlayer(0);
	if (ludum_player != nullptr)
	{
		ParticlePlayer const* player_particle = ludum_player->GetPlayerParticle();
		if (player_particle != nullptr)
			if (player_particle->level_end_reached && player_particle->level_end_timer <= 0.0f)
				return true;
	}
	return false;
}

bool LudumLevelInstance::Initialize(chaos::Game * in_game, chaos::Level * in_level)
{
	if (!chaos::TMLevelInstance::Initialize(in_game, in_level))
		return false;
	return true;
}

chaos::PlayerPawn* LudumLevelInstance::CreatePlayerPawnAtPlayerStart(chaos::Player* player, chaos::TMPlayerStart* player_start)
{
	chaos::PlayerPawn* result = chaos::TMLevelInstance::CreatePlayerPawnAtPlayerStart(player, player_start);
	if (result != nullptr)
	{
		chaos::ParticleAllocationBase* allocation = result->GetAllocation();
		if (allocation != nullptr)
		{
			LudumGame* ludum_game = GetGame();
			if (ludum_game != nullptr)
			{
				chaos::ParticleAccessor<ParticlePlayer> particles = allocation->GetParticleAccessor();
				for (ParticlePlayer& particle : particles)
				{
					float radius = chaos::GetInnerSphere(particle.bounding_box).radius;

					particle.attraction_minradius = radius + ludum_game->player_attraction_minradius;
					particle.attraction_maxradius = radius + ludum_game->player_attraction_maxradius;
					particle.attraction_force = ludum_game->player_attraction_force;
					particle.repulsion_force = ludum_game->player_repulsion_force;
					particle.tangent_force = ludum_game->player_tangent_force;
					particle.life = player->GetMaxHealth(); // XXX : the health of the player is never modified. Instead, the player particle health is changed
				}
			}
		}
	}
	return result;
}
