#include "Ludum48PlayerDisplacementComponent.h"

#include "Ludum48LevelInstance.h"

bool LudumPlayerDisplacementComponent::DoTick(float delta_time)
{
	// early exit
	chaos::PlayerPawn* pawn = player->GetPawn();
	if (pawn == nullptr)
		return true;

	LudumLevelInstance* ludum_level_instance = GetLevelInstance();
	if (ludum_level_instance == nullptr)
		return true;

	ParticlePlayer* particle = pawn->GetParticle<ParticlePlayer>(0);
	if (particle == nullptr)
		return true;

	// get player inputs of interrests
	glm::vec2 stick_position = player->GetLeftStickPosition();
	stick_position.x = chaos::MathTools::AnalogicToDiscret(stick_position.x);
	stick_position.y = chaos::MathTools::AnalogicToDiscret(stick_position.y);
	if (std::abs(stick_position.x) > std::abs(stick_position.y))
		stick_position.y = 0.0f;
	else
		stick_position.x = 0.0f;

	// get player position
	chaos::box2 initial_pawn_box = pawn->GetBoundingBox();

	chaos::box2 pawn_box = initial_pawn_box;
	glm::vec2& pawn_position = pawn_box.position;


	GridInfo& grid_info = ludum_level_instance->GetGridInfo();

	// change pawn direction
	if (particle->direction.x == 0.0f && particle->direction.y == 0.0f)
	{
		if (stick_position != glm::vec2(0.0f, 0.0f))
		{



		}

	}
	// update pawn position
	else
	{



	}



	// start displacement in given direction
	if (pawn_resting && stick_position != glm::vec2(0.0f, 0.0f))
	{
		start_pawn_position = pawn_position;
		pawn_velocity = stick_position;
		pawn_offset = 0.0f;
		pawn_resting = false;

		GridInfo& grid_info = ludum_level_instance->GetGridInfo();
		



	}

	// update displacement
	if (!pawn_resting)
	{
		for (size_t axis : {0, 1})
		{
			if (pawn_velocity[axis] == 0.0f)
				continue;
			pawn_offset = std::min(pawn_offset + pawn_max_velocity[axis] * delta_time, 1.0f);
		}

		pawn_position = start_pawn_position + pawn_offset * pawn_velocity * tile_size;
			
		if (pawn_offset == 1.0f)
			pawn_resting = true;



		// update the player pawn
		pawn->SetPosition(pawn_box.position);
	}

	return true;
}


bool LudumPlayerDisplacementComponent::SerializeIntoJSON(nlohmann::json& json_entry) const
{
	if (!chaos::PlayerDisplacementComponent::SerializeIntoJSON(json_entry))
		return false;
	chaos::JSONTools::SetAttribute(json_entry, "pawn_max_velocity", pawn_max_velocity);
	return true;
}

bool LudumPlayerDisplacementComponent::SerializeFromJSON(nlohmann::json const& json_entry)
{
	if (!chaos::PlayerDisplacementComponent::SerializeFromJSON(json_entry))
		return false;

	chaos::JSONTools::GetAttribute(json_entry, "pawn_max_velocity", pawn_max_velocity);
	return true;
}

