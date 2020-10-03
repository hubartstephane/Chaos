#include "Ludum47Level.h"
#include "Ludum47LevelInstance.h"
#include "Ludum47Game.h"
#include "Ludum47Player.h"
#include "Ludum47GameInstance.h"
#include "Ludum47PlayerDisplacementComponent.h"
#include "Ludum47Particles.h"

#include <chaos/GLMTools.h>
#include <chaos/ParticleDefault.h>
#include <chaos/GeometryFramework.h>

#include <death/FollowPlayerCameraComponent.h>
#include <death/ShakeCameraComponent.h>
#include <death/SoundListenerCameraComponent.h>


bool LudumCameraComponent::DoTick(float delta_time)
{
	if (!death::CameraComponent::DoTick(delta_time))
		return false;



	LudumPlayer const* player = GetPlayer(0);
	if (player != nullptr)
	{
		ParticlePlayer const* particle = player->GetPlayerParticle();
		if (particle)
		{
			float target_zoom = chaos::MathTools::RemapRanges(0.0f, player->max_velocity, min_zoom, max_zoom, particle->velocity);

			if (target_zoom > zoom)
				zoom += zoom_increase * delta_time;
			else
				zoom -= zoom_decrease * delta_time;
		}
	}

	zoom = std::clamp(zoom, min_zoom, max_zoom);
	return true;
}

chaos::box2 LudumCameraComponent::ApplyModifier(chaos::box2 const& src) const
{
	chaos::box2 result = src;
	result.half_size *= zoom;
	return result;
}

// =============================================================
// LudumLevelInstance implementation
// =============================================================

LudumLevelInstance::LudumLevelInstance()
{
	player_displacement_component_class = LudumPlayerDisplacementComponent::GetStaticClass();
}

void LudumLevelInstance::CreateCameras()
{
	death::TMLevelInstance::CreateCameras();

	size_t camera_count = cameras.size();
	for (size_t i = 0; i < camera_count; ++i)
	{
		cameras[i]->SetSafeZone(glm::vec2(0.0f, 0.0f));
		cameras[i]->AddComponent(new death::FollowPlayerCameraComponent(0));
		cameras[i]->AddComponent(new death::ShakeCameraComponent(0.15f, 0.05f, 0.15f, true, true));
		cameras[i]->AddComponent(new death::SoundListenerCameraComponent());

		cameras[i]->AddComponent(new LudumCameraComponent());		
	}
}

bool LudumLevelInstance::DoTick(float delta_time)
{
	death::TMLevelInstance::DoTick(delta_time);



	return true;
}

bool LudumLevelInstance::Initialize(death::Game * in_game, death::Level * in_level)
{
	if (!death::TMLevelInstance::Initialize(in_game, in_level))
		return false;

	road = FindObject("TheRoad");

	LudumLevel* ludum_level = auto_cast(in_level);
	if (ludum_level != nullptr)
	{






	}	
	return true;
}


bool LudumLevelInstance::IsPlayerDead(death::Player* player)
{
	if (death::TMLevelInstance::IsPlayerDead(player))
		return true;

	LudumPlayer* ludum_player = auto_cast(player);

	LudumLevel * ludum_level = GetLevel();

	if (ludum_level != nullptr && ludum_player != nullptr)
	{



	}
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

glm::ivec2 LudumLevelInstance::GetPlayerRacePosition(LudumPlayer const* player) const
{
	glm::ivec2 result = { 0, 0 }; 

	size_t point_count = road->points.size();
	if (point_count == 0)
		return result;

	death::TMLayerInstance const* li = FindLayerInstance("Opponents");
	if (li == nullptr)
		return result;

	if (player->GetPawn() == nullptr)
		return result;

	death::TMParticle const* particle_player = player->GetPawn()->GetParticle<death::TMParticle>(0);
	if (particle_player == nullptr)
		return result;

	size_t count = li->GetObjectCount();
	for (size_t i = 0; i < count; ++i)
	{
		LudumOpponent const* opponent = auto_cast(li->GetObject(i));
		if (opponent == nullptr)
			continue;

		death::TMParticle const* particle_opponent = opponent->GetParticle<death::TMParticle>(0);
		if (particle_opponent == nullptr)
			continue;
		
		// alive opponent
		++result.y;

		if (player->race_position < opponent->race_position)
			++result.x;
		else if (player->race_position == opponent->race_position)
		{
			glm::vec2 target = road->points[(opponent->race_position.current_road_point + 1) % point_count].position;

			if (glm::length2(target - particle_player->bounding_box.position) > glm::length2(target - particle_opponent->bounding_box.position))
				++result.x;
		}
	}
	return result;
}
