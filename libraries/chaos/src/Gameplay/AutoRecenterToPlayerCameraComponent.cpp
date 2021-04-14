#include <chaos/Chaos.h>

namespace chaos
{
	// =============================================
	// AutoRecenterToPlayerCameraComponent
	// =============================================

	bool AutoRecenterToPlayerCameraComponent::DoTick(float delta_time)
	{
		CameraComponent::DoTick(delta_time);

		// 1.1 just to be sure that the space between the 2 limits is greater that pawn size
		float constexpr PAWNBOX_INCREASE_FACTOR = 1.1f;


		// get the wanted player
		Player* player = GetPlayer(player_index);
		if (player == nullptr)
			return true;

		// get camera, cannot continue if it is empty
		box2 camera_box = camera->GetCameraBox(false);
		if (IsGeometryEmpty(camera_box))
			return true;

		box2 world = GetLevelInstance()->GetBoundingBox();

		// keep pawn inside camera safe zone
		PlayerPawn const* player_pawn = player->GetPawn();
		if (player_pawn != nullptr)
		{
			box2 pawn_box = player_pawn->GetBoundingBox();
			if (!IsGeometryEmpty(pawn_box))
			{

				glm::vec2 safe_zone = camera->GetSafeZone();

				// initialize previous frame position if not initialized
				if (isnan(pawn_previous_position[0]) || isnan(pawn_previous_position[1]))
					pawn_previous_position = pawn_box.position;

				// fake a manual camera with stick displacement by enlarging the pawn_box
				glm::vec2 right_stick = player->GetRightStickPosition();
				if (right_stick.x != 0.0f || right_stick.y != 0.0f)
				{
					idle_timer = 0.0f; // no automatic recenter

					camera_offset += right_stick * camera_offset_speed * camera_box.half_size * 2.0f * safe_zone * delta_time;

					std::pair<glm::vec2, glm::vec2> pawn_corners = GetBoxCorners(pawn_box);
					for (int axis = 0; axis < 2; ++axis)
					{
						if (camera_offset[axis] > 0) // use left/bottom corner as a reference
						{
							float new_corner = std::min(pawn_corners.second[axis] + camera_offset[axis], pawn_corners.first[axis] + safe_zone[axis] * camera_box.half_size[axis] * 1.9f);
							if (!IsGeometryEmpty(world))
								new_corner = std::min(new_corner, world.position[axis] + world.half_size[axis]);

							camera_offset[axis] = new_corner - pawn_corners.second[axis];
							pawn_corners.second[axis] = new_corner;
						}
						else if (camera_offset[axis] < 0) // use right/to corner as a reference
						{
							float new_corner = std::max(pawn_corners.first[axis] + camera_offset[axis], pawn_corners.second[axis] - safe_zone[axis] * camera_box.half_size[axis] * 1.9f);
							if (!IsGeometryEmpty(world))
								new_corner = std::max(new_corner, world.position[axis] - world.half_size[axis]);

							camera_offset[axis] = new_corner - pawn_corners.first[axis];
							pawn_corners.first[axis] = new_corner;
						}

						if (camera_offset[axis] < 0.0)
							idle_timer = idle_timer;
						if (camera_offset[axis] > 0.0)
							idle_timer = idle_timer;




					}
					pawn_box = box2(pawn_corners); // hack the pawn size

				}





				


















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
				glm::bvec2 idle = { false, false };


				glm::vec2 pawn_box_ratio = PAWNBOX_INCREASE_FACTOR * pawn_box.half_size / (camera_box.half_size * safe_zone);

				for (int axis = 0; axis < 2; ++axis)
				{
					float pawn_speed = std::abs(pawn_previous_position[axis] - pawn_box.position[axis]) / delta_time;

					if (pawn_speed > fast_pawn_limit)
					{
						idle_timer = 0.0f;

						if (PAWNBOX_INCREASE_FACTOR * pawn_box.half_size[axis] < camera_box.half_size[axis])
						{
							// player going right/up
							if (pawn_previous_position[axis] < pawn_box.position[axis])
							{
								min_dynamic_safe_zone[axis] = std::max(min_dynamic_safe_zone[axis] - safe_zone_speed * delta_time, 0.0f);
								max_dynamic_safe_zone[axis] = std::min(max_dynamic_safe_zone[axis] + safe_zone_speed * delta_time, 1.0f - pawn_box_ratio[axis]);

							}
							// player going left/down
							else if (pawn_previous_position[axis] > pawn_box.position[axis])
							{
								min_dynamic_safe_zone[axis] = std::min(min_dynamic_safe_zone[axis] + safe_zone_speed * delta_time, 1.0f - pawn_box_ratio[axis]);
								max_dynamic_safe_zone[axis] = std::max(max_dynamic_safe_zone[axis] - safe_zone_speed * delta_time, 0.0f);
							}
						}
					}
					// player not moving (limits goes to their initial state : 0)
					else
					{
						idle[axis] = true;
					}
				}


				// step 2 : recenter the camera if both idle for both axis
				if (idle[0] && idle[1])
				{
					if (right_stick.x == 0.0f && right_stick.y == 0.0f)
					{
						idle_timer = std::min(idle_timer + delta_time, idle_delay);
						if (idle_timer >= idle_delay)
						{
							for (int axis = 0; axis < 2; ++axis)
							{
								float target_safe_zone = 0.5f - pawn_box_ratio[axis] * 0.5f;
								if (min_dynamic_safe_zone[axis] < target_safe_zone)
									min_dynamic_safe_zone[axis] = std::min(min_dynamic_safe_zone[axis] + safe_zone_speed * delta_time, target_safe_zone);
								else
									min_dynamic_safe_zone[axis] = std::max(min_dynamic_safe_zone[axis] - safe_zone_speed * delta_time, target_safe_zone);

								if (max_dynamic_safe_zone[axis] < target_safe_zone)
									max_dynamic_safe_zone[axis] = std::min(max_dynamic_safe_zone[axis] + safe_zone_speed * delta_time, target_safe_zone);
								else
									max_dynamic_safe_zone[axis] = std::max(max_dynamic_safe_zone[axis] - safe_zone_speed * delta_time, target_safe_zone);
							}
						}
					}
				}
				// player still moving
				else
				{
					idle_timer = 0;

					camera_offset.x = MathTools::TargetValue(camera_offset.x, 0.0f, camera_offset_speed, camera_offset_speed);
					camera_offset.y = MathTools::TargetValue(camera_offset.y, 0.0f, camera_offset_speed, camera_offset_speed);

					for (int axis = 0; axis < 2; ++axis)
					{
						if (idle[axis])
						{
							min_dynamic_safe_zone[axis] = std::max(min_dynamic_safe_zone[axis] - safe_zone_speed * delta_time, 0.0f);
							max_dynamic_safe_zone[axis] = std::max(max_dynamic_safe_zone[axis] - safe_zone_speed * delta_time, 0.0f);
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
		if (!IsGeometryEmpty(world))
			RestrictToInside(world, camera_box, false);

		// apply camera changes
		camera->SetCameraBox(camera_box);

		return true;
	}

	bool AutoRecenterToPlayerCameraComponent::SerializeIntoJSON(nlohmann::json& json_entry) const
	{
		if (!CameraComponent::SerializeIntoJSON(json_entry))
			return false;
		JSONTools::SetAttribute(json_entry, "fast_pawn_limit", fast_pawn_limit);
		JSONTools::SetAttribute(json_entry, "safe_zone_speed", safe_zone_speed);
		JSONTools::SetAttribute(json_entry, "camera_offset_speed", camera_offset_speed);
		JSONTools::SetAttribute(json_entry, "idle_delay", idle_delay);
		return true;
	}

	bool AutoRecenterToPlayerCameraComponent::SerializeFromJSON(nlohmann::json const& json_entry)
	{
		if (!CameraComponent::SerializeFromJSON(json_entry))
			return false;
		JSONTools::GetAttribute(json_entry, "fast_pawn_limit", fast_pawn_limit);
		JSONTools::GetAttribute(json_entry, "safe_zone_speed", safe_zone_speed);
		JSONTools::GetAttribute(json_entry, "camera_offset_speed", camera_offset_speed);
		JSONTools::GetAttribute(json_entry, "idle_delay", idle_delay);
		return true;
	}

}; // namespace chaos
