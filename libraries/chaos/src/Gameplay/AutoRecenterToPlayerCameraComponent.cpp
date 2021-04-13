#include <chaos/Chaos.h>

namespace chaos
{
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
				glm::vec2 safe_zone = camera->GetSafeZone();

				// XXX : The safe zone used to restrict the player inside the camera view is dynamic
				//       for each axis, we have 2 invisible limits for the camera view
				//       one left, one right (resp. up and down)
				//       the limits ajusts themselves in the opposite direction of the player
				//       (so that when the player goes left, he sees more to the left)
				//
				//  +------------------+
				//  |                  |    P : player is going left
				//  |      |->  <-P  | |
				//  |      |         | |    The min limit is going left so that the player is restricted to the right portion of the screen
				//  |      +---------+ |
				//  |   min limit      |
				//
				//  min_dynamic_safe_zone : 0 for initial value. Increase to 1 whenever the player goes left/down
				//  max_dynamic_safe_zone : 0 for initial value. Increase to 1 whenever the player goes right/up
				// 
				for (int axis = 0; axis < 2; ++axis)
				{
					if (!isnan(pawn_previous_position[axis]))
					{
						if (pawn_box.half_size[axis] < camera_box.half_size[axis])
						{
							float pawn_box_ratio = 1.2f * pawn_box.half_size[axis] / (camera_box.half_size[axis] * safe_zone[axis]);

							// player going right/up
							if (pawn_previous_position[axis] < pawn_box.position[axis])
							{
								min_dynamic_safe_zone[axis] = std::max(min_dynamic_safe_zone[axis] - safe_zone_speed * delta_time, 0.0f);
								max_dynamic_safe_zone[axis] = std::min(max_dynamic_safe_zone[axis] + safe_zone_speed * delta_time, 1.0f - pawn_box_ratio);

							}
							// player going left/down
							else if (pawn_previous_position[axis] > pawn_box.position[axis])
							{
								min_dynamic_safe_zone[axis] = std::min(min_dynamic_safe_zone[axis] + safe_zone_speed * delta_time, 1.0f - pawn_box_ratio);
								max_dynamic_safe_zone[axis] = std::max(max_dynamic_safe_zone[axis] - safe_zone_speed * delta_time, 0.0f);
							}
							// player not moving (limits goes to their initial state : 0)
							else
							{
								min_dynamic_safe_zone[axis] = std::max(min_dynamic_safe_zone[axis] - safe_zone_speed * delta_time, 0.0f);
								max_dynamic_safe_zone[axis] = std::max(max_dynamic_safe_zone[axis] - safe_zone_speed * delta_time, 0.0f);
							}
						}
					}
				}
				
				// store the pawn previous position for next frame
				pawn_previous_position = pawn_box.position;
				
				// min_ratio and max_ratio are the ratio of the limits for the whole camera_box
				glm::vec2 border_ratio = (glm::vec2(1.0f, 1.0f) - safe_zone) * 0.5f;			
				glm::vec2 min_ratio = border_ratio + safe_zone * min_dynamic_safe_zone;
				glm::vec2 max_ratio = glm::vec2(1.0f, 1.0f) - border_ratio - safe_zone * max_dynamic_safe_zone;

				// min/max position of the dynamic safe_zone
				glm::vec2 min_position = camera_box.position - camera_box.half_size + 2.0f * min_ratio * camera_box.half_size;
				glm::vec2 max_position = camera_box.position - camera_box.half_size + 2.0f * max_ratio * camera_box.half_size;

				// compute the dynamic safe zone
				box2 safe_camera = box2(std::make_pair(max_position, min_position));
				glm::vec2 safe_camera_position = safe_camera.position;

				// apply the dynamic safe zone displacement to the real camera box
				if (RestrictToInside(safe_camera, pawn_box, true)) 
					camera_box.position += (safe_camera.position - safe_camera_position);
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
		
		JSONTools::SetAttribute(json_entry, "safe_zone_speed", safe_zone_speed);
		JSONTools::SetAttribute(json_entry, "recenter_speed", recenter_speed);
		JSONTools::SetAttribute(json_entry, "idle_delay", idle_delay);
		return true;
	}

	bool AutoRecenterToPlayerCameraComponent::SerializeFromJSON(nlohmann::json const& json_entry)
	{
		if (!CameraComponent::SerializeFromJSON(json_entry))
			return false;
		JSONTools::GetAttribute(json_entry, "safe_zone_speed", safe_zone_speed);
		JSONTools::GetAttribute(json_entry, "recenter_speed", recenter_speed);
		JSONTools::GetAttribute(json_entry, "idle_delay", idle_delay);
		return true;
	}

}; // namespace chaos
