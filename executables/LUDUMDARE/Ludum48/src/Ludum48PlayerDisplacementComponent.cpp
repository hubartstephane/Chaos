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

	GridInfo& grid_info = ludum_level_instance->GetGridInfo();
	if (grid_info.cells == nullptr)
		return true;


	chaos::Key const fake_displacement_key_buttons[] = { chaos::KeyboardButton::LEFT_CONTROL, chaos::KeyboardButton::RIGHT_CONTROL, chaos::GamepadButton::A, chaos::Key() };
	bool fake_displacement = GetPlayer()->CheckButtonPressed(fake_displacement_key_buttons);

	if (fake_displacement)
		fake_displacement = true;


	// get player inputs of interrests
	glm::vec2 stick_position = player->GetLeftStickPosition();
	stick_position.x = chaos::MathTools::AnalogicToDiscret(stick_position.x);
	stick_position.y = chaos::MathTools::AnalogicToDiscret(stick_position.y);
	if (std::abs(stick_position.x) > std::abs(stick_position.y))
		stick_position.y = 0.0f;
	else
		stick_position.x = 0.0f;



	// change pawn direction
	if (particle->direction.x == 0.0f && particle->direction.y == 0.0f)
	{
		if (stick_position != glm::vec2(0.0f, 0.0f))
		{
			glm::ivec2 p = grid_info.GetIndexForPosition(particle->bounding_box.position);

			bool inside_world = false;
			if (stick_position.x > 0 && p.x < grid_info.size.x - 1)
				inside_world = true;
			else if (stick_position.x < 0 && p.x > 0.0f)
				inside_world = true;
			else if (stick_position.y > 0 && p.y < grid_info.size.y - 1)
				inside_world = true;
			else if (stick_position.y < 0 && p.y > 0.0f)
				inside_world = true;

			if (inside_world)
			{
				GridCellInfo& other = grid_info(p + chaos::RecastVector<glm::ivec2>(stick_position));


				bool can_go = false;

				if (other.particle != nullptr)
				{
					if (other.particle->type == GameObjectType::Foam)
					{
						other.particle->destroy_particle = true;
						if (!fake_displacement)
							other.particle = nullptr;
						can_go = true;
					}
					else if (other.particle->type == GameObjectType::Diamond)
					{
						other.particle->destroy_particle = true;
						ludum_level_instance->TakeDiamond();
						if (!fake_displacement)
							other.particle = nullptr;
						can_go = true;
					}
					else if (other.particle->type == GameObjectType::Rock)
					{
						if (stick_position.x != 0.0f && other.particle->direction == glm::vec2(0.0f, 0.0f)) // can only push horizontally
						{
							glm::ivec2 rock_p = grid_info.GetCellCoord(other);
							if ((rock_p.x > 0 && stick_position.x < 0.0f) || (rock_p.x < grid_info.size.x - 1 && stick_position.x > 0.0f))
							{
								GridCellInfo& next_to_rock = grid_info(rock_p + chaos::RecastVector<glm::ivec2>(stick_position));
								if (next_to_rock.CanLock(other.particle))
								{
									next_to_rock.Lock(other.particle);
									other.particle->direction = stick_position;
									other.particle->speed = push_speed;
									particle->direction = stick_position;
									particle->speed = push_speed;
								}
							}
						}
					}
				}
				else
					can_go = true;



				if (can_go && other.CanLock(particle))
				{
					if (!fake_displacement)
					{
						other.Lock(particle);
						particle->direction = stick_position;
						particle->speed = pawn_speed;
					}

				}
				




			}
		}

	}
	// update pawn position
	else
	{
		UpdateParticlePositionInGrid(particle, delta_time, grid_info);
		
		

		



	}
	//

	


	return true;
}


bool LudumPlayerDisplacementComponent::SerializeIntoJSON(nlohmann::json& json_entry) const
{
	if (!chaos::PlayerDisplacementComponent::SerializeIntoJSON(json_entry))
		return false;
	chaos::JSONTools::SetAttribute(json_entry, "pawn_speed", pawn_speed);
	chaos::JSONTools::SetAttribute(json_entry, "push_speed", push_speed);
	return true;
}

bool LudumPlayerDisplacementComponent::SerializeFromJSON(nlohmann::json const& json_entry)
{
	if (!chaos::PlayerDisplacementComponent::SerializeFromJSON(json_entry))
		return false;

	chaos::JSONTools::GetAttribute(json_entry, "pawn_speed", pawn_speed);
	chaos::JSONTools::GetAttribute(json_entry, "push_speed", push_speed);
	return true;
}

