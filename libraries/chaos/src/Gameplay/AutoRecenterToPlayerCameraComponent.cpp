#include <chaos/Chaos.h>

namespace chaos
{
	float GMN = 0.0f;
	float GMX = 0.0f;

	// =============================================
	// AutoRecenterToPlayerCameraComponent
	// =============================================

	box2 AutoRecenterToPlayerCameraComponent::ApplyModifier(box2 const& src) const
	{
		box2 result = src;
		result.position += offset;
		return result;
	}

	bool AutoRecenterToPlayerCameraComponent::DoTick(float delta_time)
	{
		CameraComponent::DoTick(delta_time);

		// get the wanted player
		Player* player = GetPlayer(player_index);
		if (player == nullptr)
			return true;

		// get camera, cannot continue if it is empty
		box2 camera_box = camera->GetCameraBox(false);
		if (IsGeometryEmpty(camera_box))
			return true;

		// keep pawn inside camera safe zone
		PlayerPawn const* player_pawn = player->GetPawn();
		if (player_pawn != nullptr)
		{
			box2 pawn_box = player_pawn->GetBoundingBox();
			if (!IsGeometryEmpty(pawn_box))
			{
				float speed = 0.25f; //safe_zone_speed;

				glm::vec2 safe_zone = camera->GetSafeZone();

				for (int axis = 0; axis < 2; ++axis)
				{
					if (!isnan(pawn_previous_position[axis]))
					{
						if (pawn_box.half_size[axis] < camera_box.half_size[axis])
						{
							float pawn_box_ratio = 1.2f * pawn_box.half_size[axis] / (camera_box.half_size[axis] * safe_zone[axis]);

							if (pawn_previous_position[axis] < pawn_box.position[axis])
							{
								min_dynamic_safe_zone[axis] = std::max(min_dynamic_safe_zone[axis] - speed * delta_time, 0.0f);
								max_dynamic_safe_zone[axis] = std::min(max_dynamic_safe_zone[axis] + speed * delta_time, 1.0f - pawn_box_ratio);

							}
							else if (pawn_previous_position[axis] > pawn_box.position[axis])
							{
								min_dynamic_safe_zone[axis] = std::min(min_dynamic_safe_zone[axis] + speed * delta_time, 1.0f - pawn_box_ratio);
								max_dynamic_safe_zone[axis] = std::max(max_dynamic_safe_zone[axis] - speed * delta_time, 0.0f);
							}
							else
							{
								min_dynamic_safe_zone[axis] = std::max(min_dynamic_safe_zone[axis] - speed * delta_time, 0.0f);
								max_dynamic_safe_zone[axis] = std::max(max_dynamic_safe_zone[axis] - speed * delta_time, 0.0f);
							}
						}
					}

					if (axis == 0)
					{

					}


				}
				
				pawn_previous_position = pawn_box.position;
				

				glm::vec2 border_ratio = (glm::vec2(1.0f, 1.0f) - safe_zone) * 0.5f;
			
				glm::vec2 mn = border_ratio + safe_zone * min_dynamic_safe_zone;
				glm::vec2 mx = glm::vec2(1.0f, 1.0f) - border_ratio - safe_zone * max_dynamic_safe_zone;








				box2 safe_camera = camera_box;
				//safe_camera.half_size *= camera->GetSafeZone();

			//	glm::vec2 min_position = safe_camera.position - 0.5f * safe_zone * safe_camera.half_size + mn * safe_zone * safe_camera.half_size;
			//	glm::vec2 max_position = safe_camera.position + 0.5f * safe_zone * safe_camera.half_size - mx * safe_zone * safe_camera.half_size;

				glm::vec2 min_position = safe_camera.position - safe_camera.half_size + 2.0f * mn * safe_camera.half_size;
				glm::vec2 max_position = safe_camera.position - safe_camera.half_size + 2.0f * mx * safe_camera.half_size;




				GMN = max_position.x - min_position.x;
				GMX = mx.x;


				safe_camera = box2(std::make_pair(max_position, min_position));

				glm::vec2 s = safe_camera.position;

				if (RestrictToInside(safe_camera, pawn_box, true)) // apply the safe_zone displacement to the real camera
					camera_box.position += (safe_camera.position - s);
			}
		}

		// try to keep the camera in the world
		box2 world = GetLevelInstance()->GetBoundingBox();
		if (!IsGeometryEmpty(world))
			RestrictToInside(world, camera_box, false);

		// apply camera changes
		camera->SetCameraBox(camera_box);

		return true;
	}












#if 0


	bool AutoRecenterToPlayerCameraComponent::DoTick(float delta_time)
	{
		CameraComponent::DoTick(delta_time);

		Player* player = GetPlayer(player_index);
		if (player == nullptr)
			return true;

		glm::vec2 right_stick = player->GetRightStickPosition();

		offset += right_stick * delta_time * 150.0f;


#if 0

		// get the wanted player
		Player * player = camera->GetPlayer(player_index);
		if (player == nullptr)
			return true;

		// get camera, cannot continue if it is empty
		box2 camera_box = camera->GetCameraBox(false);
		if (IsGeometryEmpty(camera_box))
			return true;

		// keep pawn inside camera safe zone
		PlayerPawn const* player_pawn = player->GetPawn();
		if (player_pawn != nullptr)
		{
			box2 pawn_box = player_pawn->GetBoundingBox();
			if (!IsGeometryEmpty(pawn_box))
			{
				box2 safe_camera = camera_box;
				safe_camera.half_size *= camera->GetSafeZone();

				if (RestrictToInside(safe_camera, pawn_box, true)) // apply the safe_zone displacement to the real camera
					camera_box.position = safe_camera.position;
			}
		}

		// try to keep the camera in the world
		box2 world = camera->GetLevelInstance()->GetBoundingBox();
		if (!IsGeometryEmpty(world))
			RestrictToInside(world, camera_box, false);

		// apply camera changes
		camera->SetCameraBox(camera_box);
#endif
		return true;
	}

#endif

	bool AutoRecenterToPlayerCameraComponent::SerializeIntoJSON(nlohmann::json& json_entry) const
	{
		if (!CameraComponent::SerializeIntoJSON(json_entry))
			return false;
		JSONTools::SetAttribute(json_entry, "recenter_speed", recenter_speed);
		JSONTools::SetAttribute(json_entry, "idle_delay", idle_delay);
		return true;
	}

	bool AutoRecenterToPlayerCameraComponent::SerializeFromJSON(nlohmann::json const& json_entry)
	{
		if (!CameraComponent::SerializeFromJSON(json_entry))
			return false;
		JSONTools::GetAttribute(json_entry, "recenter_speed", recenter_speed);
		JSONTools::GetAttribute(json_entry, "idle_delay", idle_delay);
		return true;
	}

}; // namespace chaos
