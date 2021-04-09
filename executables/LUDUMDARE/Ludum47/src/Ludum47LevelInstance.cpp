#include <chaos/Chaos.h>

#include "Ludum47Level.h"
#include "Ludum47LevelInstance.h"
#include "Ludum47Game.h"
#include "Ludum47Player.h"
#include "Ludum47GameInstance.h"
#include "Ludum47PlayerDisplacementComponent.h"
#include "Ludum47Particles.h"

bool LudumCameraComponent::DoTick(float delta_time)
{
	if (!chaos::CameraComponent::DoTick(delta_time))
		return false;






	LudumPlayer const* player = GetPlayer(0);
	if (player != nullptr)
	{
		ParticlePlayer const* particle = player->GetPlayerParticle();
		if (particle)
		{
			float target_zoom = chaos::MathTools::RemapRanges(0.0f, player->car_data.max_velocity, min_zoom, max_zoom, particle->velocity);

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

void LudumLevelInstance::CreateCameraComponents(chaos::Camera* camera, chaos::TMCameraTemplate* camera_template)
{
	chaos::TMLevelInstance::CreateCameraComponents(camera, camera_template);

	camera->SetSafeZone(glm::vec2(0.0f, 0.0f));
	camera->AddComponent(new chaos::FollowPlayerCameraComponent(0));
	camera->AddComponent(new chaos::ShakeCameraComponent(0.15f, 0.05f, 0.15f, true, true));
	camera->AddComponent(new chaos::SoundListenerCameraComponent());
	camera->AddComponent(new LudumCameraComponent());
}

bool LudumLevelInstance::DoTick(float delta_time)
{
	chaos::TMLevelInstance::DoTick(delta_time);

	if (effective_start_timer > 0.0f)
	{
		float new_value = std::max(0.0f, effective_start_timer - delta_time);

		bool sound_played = false;

		if (effective_start_timer <= 3.0f)
		{
			if (int(effective_start_timer) != int(new_value))
			{

				GetGame()->PlaySound("Start1", false, false, 0, chaos::SoundContext::GAME);
				sound_played = true;
			}
		}
		effective_start_timer = new_value;
		if (effective_start_timer == 0.0f && !sound_played)
			GetGame()->PlaySound("Start2", false, false, 0, chaos::SoundContext::GAME);
	}


	if (completion_timer > 0.0f)
		completion_timer = std::max(0.0f, completion_timer - delta_time);

	if (lost_timer > 0.0f)
		lost_timer = std::max(0.0f, lost_timer - delta_time);

	

	return true;
}

bool LudumLevelInstance::Initialize(chaos::Game * in_game, chaos::Level * in_level)
{
	if (!chaos::TMLevelInstance::Initialize(in_game, in_level))
		return false;

	road = FindObject("TheRoad", true);

	LudumLevel* ludum_level = auto_cast(in_level);
	if (ludum_level != nullptr)
	{






	}	
	return true;
}


bool LudumLevelInstance::IsPlayerDead(chaos::Player* player)
{
	if (chaos::TMLevelInstance::IsPlayerDead(player))
		return true;

	if (lost_timer == 0.0f)
		return true;



	LudumPlayer* ludum_player = auto_cast(player);

	LudumLevel * ludum_level = GetLevel();

	if (ludum_level != nullptr && ludum_player != nullptr)
	{



	}
	return false;
}

void LudumLevelInstance::OnOpponentArrived()
{

	// stop counting if player is arrived
	LudumPlayer const* ludum_player = GetPlayer(0);
	if (ludum_player != nullptr)
		if (!ludum_player->race_position.IsCompleted())
			++opponent_arrived_count;


	if (opponent_arrived_count > opponent_count / 2)
		lost_timer = 2.0f;


}

bool LudumLevelInstance::CheckLevelCompletion() const
{
	LudumLevel const * ludum_level = GetLevel();

	LudumPlayer const * ludum_player = GetPlayer(0);
	if (ludum_player != nullptr && ludum_level != nullptr)
	{
		if (lost_timer < 0.0f && ludum_player->race_position.IsCompleted())
		{
			if (completion_timer < 0.0f)
			{
				completion_timer = completion_delay; // forced to be mutable !??
			}
			return true;
		}
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

	chaos::TMLayerInstance const* li = FindLayerInstance("Opponents", true);
	if (li == nullptr)
		return result;

	if (player->GetPawn() == nullptr)
		return result;

	chaos::TMParticle const* particle_player = player->GetPawn()->GetParticle<chaos::TMParticle>(0);
	if (particle_player == nullptr)
		return result;

	size_t count = li->GetObjectCount();
	for (size_t i = 0; i < count; ++i)
	{
		LudumOpponent const* opponent = auto_cast(li->GetObject(i));
		if (opponent == nullptr)
			continue;

		chaos::TMParticle const* particle_opponent = opponent->GetParticle<chaos::TMParticle>(0);
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
