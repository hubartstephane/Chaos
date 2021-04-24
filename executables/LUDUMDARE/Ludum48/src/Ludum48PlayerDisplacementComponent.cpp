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

	GridInfo& grid_info = ludum_level_instance->GetGridInfo();

	// change pawn direction
	if (particle->direction.x == 0.0f && particle->direction.y == 0.0f)
	{
		if (stick_position != glm::vec2(0.0f, 0.0f))
		{


			glm::ivec2 p = grid_info.GetIndexForPosition(particle->bounding_box.position);

			int index = p.x + p.y * grid_info.size.x;

			int other_index = -1;
			if (stick_position.x > 0 && p.x < grid_info.size.x - 1)
				other_index = index + 1;
			else if (stick_position.x < 0 && p.x > 0.0f)
				other_index = index - 1;
			else if (stick_position.y > 0 && p.y < grid_info.size.y - 1)
				other_index = index + grid_info.size.x;
			else if (stick_position.y < 0 && p.y > 0.0f)
				other_index = index - grid_info.size.x;

			if (other_index >= 0)
			{
				GridCellInfo& other = grid_info(p + chaos::RecastVector<glm::ivec2>(stick_position));
				if (other.CanLock(particle))
				{
					other.Lock(particle);
					particle->direction = stick_position;


				}
				




			}
		}

	}
	// update pawn position
	else
	{
		UpdateParticlePositionInGrid(particle, pawn_speed, delta_time, grid_info);

		



	}


	return true;
}


bool LudumPlayerDisplacementComponent::SerializeIntoJSON(nlohmann::json& json_entry) const
{
	if (!chaos::PlayerDisplacementComponent::SerializeIntoJSON(json_entry))
		return false;
	chaos::JSONTools::SetAttribute(json_entry, "pawn_speed", pawn_speed);
	return true;
}

bool LudumPlayerDisplacementComponent::SerializeFromJSON(nlohmann::json const& json_entry)
{
	if (!chaos::PlayerDisplacementComponent::SerializeFromJSON(json_entry))
		return false;

	chaos::JSONTools::GetAttribute(json_entry, "pawn_speed", pawn_speed);
	return true;
}

