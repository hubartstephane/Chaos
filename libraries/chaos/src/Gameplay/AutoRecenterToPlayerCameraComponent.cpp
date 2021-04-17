#include <chaos/Chaos.h>

namespace chaos
{
	// =============================================
	// AutoRecenterToPlayerCameraComponent
	// =============================================

	// while the pawn_box might be larger than slow_safe_box (but never than safe_box), we work with a slightly extended slow_safe_box if necessary
	box2 GetEffectiveSlowSafeBox(box2 const & slow_safe_box, box2 const& safe_box, box2 const& pawn_box)
	{
		box2 result = slow_safe_box;
		for (size_t axis : {0, 1})
		{
			float d = (pawn_box.half_size[axis] - result.half_size[axis]);
			if (d > 0.0f) // too big for that axis
			{
				result.half_size[axis] += d;

				float mx = safe_box.position[axis] + safe_box.half_size[axis];
				if (result.position[axis] + result.half_size[axis] > mx)
					result.position[axis] = mx - result.half_size[axis];
				else
				{
					float mn = safe_box.position[axis] - safe_box.half_size[axis];
					if (result.position[axis] - result.half_size[axis] < mn)
						result.position[axis] = mn + result.half_size[axis];
				}
			}
		}
		return result;
	}

	float GetNormalizedLimitValue(float origin, float t, float half_size)
	{
		return (t - origin) / (2.0f * half_size);
	}

	//                camera_box
	//  +-----------------------------------+
	//  |                                   |
	//  |             safe_box              |
	//  |   +---------------------------+   |
	//  |   |                           |   |
	//  |   |  <-+->            <-+->   |   |
	//  |   |    | slow_safe_box  |     |   |
	//  |   |    +----------------+     |   |   XX : pawn_box
	//  |   |    |       XX       |     |   |   XX
	//  |   |    |       XX       |     |   |
	//  |   |  min_limit      max_limit |   |
	//  |   |                           |   |
	//
	//
	// We considere 3 pawn speeds
	//  - fast
	//  - slow
	//  - idle
	//
	// slow speed: pawn_box is restricted inside the slow_safe_box
	//
	// fast speed: we move min_limit/max_limit so that the pawn_box is stucked at the whole safe_box borders
	//
	// idle speed: after an idle time, the pawn is centered inside the slow_safe_box
	//             the camera may be moved manually (this move min_limit/max_limit like for fast speeds)


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
				box2 safe_box = CameraTools::GetSafeCameraBox(camera_box, camera->GetSafeZone());

				// pawn is too big (in any axis) to fit inside the safe_zone, just use restrict method (with degenerated use case implemented)
				if (!glm::all(glm::greaterThanEqual(safe_box.half_size, pawn_box.half_size)))
				{
					RestrictToInside(safe_box, pawn_box, true);
				}
				// pawn fit inside the safe zone (normal case)
				else
				{
					box2 slow_safe_box = CameraTools::GetSafeCameraBox(safe_box, slow_safe_zone); // the safe box of the safe box
					slow_safe_box = GetEffectiveSlowSafeBox(slow_safe_box, safe_box, pawn_box);

					// initialize previous frame position if not initialized
					if (isnan(pawn_previous_position[0]) || isnan(pawn_previous_position[1]))
						pawn_previous_position = pawn_box.position;
					// get the right stick
					glm::vec2 right_stick = player->GetRightStickPosition();
					glm::vec2 left_stick = player->GetLeftStickPosition();

					glm::vec2 target_min_limit = { 0.0f, 0.0f }; // limit expressed with safe_zone as unit
					glm::vec2 target_max_limit = { 0.0f, 0.0f }; // 0 for limits on the slow_safe_zone

					std::pair<glm::vec2, glm::vec2> safe_corners = GetBoxCorners(safe_box);
					std::pair<glm::vec2, glm::vec2> slow_safe_corners = GetBoxCorners(slow_safe_box);

					glm::vec2 pawn_box_ratio = PAWNBOX_INCREASE_FACTOR * pawn_box.half_size / safe_box.half_size;

					// analyse the speed
					glm::vec2 wanted_limit_speed = { limit_speed, limit_speed }; // by default, considere the moving speed

					for (size_t axis : {0, 1})
					{
						float pawn_speed = std::abs(pawn_previous_position[axis] - pawn_box.position[axis]) / delta_time;
						// fast speed
						if (pawn_speed >= fast_pawn_speed)
						{
							idle_timer = 0.0f;
							// pawn going right/up
							if (pawn_previous_position[axis] < pawn_box.position[axis])
							{
								target_min_limit[axis] = GetNormalizedLimitValue(slow_safe_corners.first[axis], safe_corners.first[axis], safe_box.half_size[axis]);
								target_max_limit[axis] = GetNormalizedLimitValue(slow_safe_corners.second[axis], safe_corners.first[axis], safe_box.half_size[axis]) + pawn_box_ratio[axis];
							}
							// pawn going left/down
							else if (pawn_previous_position[axis] > pawn_box.position[axis])
							{
								target_min_limit[axis] = GetNormalizedLimitValue(slow_safe_corners.first[axis], safe_corners.second[axis], safe_box.half_size[axis]) - pawn_box_ratio[axis];
								target_max_limit[axis] = GetNormalizedLimitValue(slow_safe_corners.second[axis], safe_corners.second[axis], safe_box.half_size[axis]);
							}
						}
						// camera manually displaced to the right/up
						else if (right_stick[axis] > 0.0f)
						{
							idle_timer = 0.0f;
							wanted_limit_speed[axis] = manual_limit_speed;
							target_min_limit[axis] = GetNormalizedLimitValue(slow_safe_corners.first[axis], safe_corners.first[axis], safe_box.half_size[axis]);
							target_max_limit[axis] = GetNormalizedLimitValue(slow_safe_corners.second[axis], safe_corners.first[axis], safe_box.half_size[axis]) + pawn_box_ratio[axis];
						}
						// camera manually displaced to the left/down
						else if (right_stick[axis] < 0.0f)
						{
							idle_timer = 0.0f;
							wanted_limit_speed[axis] = manual_limit_speed;
							target_min_limit[axis] = GetNormalizedLimitValue(slow_safe_corners.first[axis], safe_corners.second[axis], safe_box.half_size[axis]) - pawn_box_ratio[axis];
							target_max_limit[axis] = GetNormalizedLimitValue(slow_safe_corners.second[axis], safe_corners.second[axis], safe_box.half_size[axis]);
						}
						else if (pawn_speed <= idle_pawn_speed)
						{
							wanted_limit_speed[axis] = recenter_limit_speed;
							target_min_limit[axis] = min_dynamic_safe_zone[axis];
							target_max_limit[axis] = max_dynamic_safe_zone[axis];
						}
						else
						{
							// nop: targe is already 0 
						}
					}

					// autocenter
					idle_timer = std::min(idle_timer + delta_time, idle_delay);
					if (idle_timer >= idle_delay)
					{
						for (size_t axis : {0, 1})
						{
							target_min_limit[axis] = GetNormalizedLimitValue(slow_safe_corners.first[axis], slow_safe_box.position[axis] - pawn_box.half_size[axis], safe_box.half_size[axis]);
							target_max_limit[axis] = GetNormalizedLimitValue(slow_safe_corners.second[axis], slow_safe_box.position[axis] + pawn_box.half_size[axis], safe_box.half_size[axis]);
						}
					}

					// compute dynamic limit by interpolation
					for (size_t axis : {0, 1})
					{
						float delta_limit = wanted_limit_speed[axis] * delta_time;
						min_dynamic_safe_zone[axis] = MathTools::TargetValue(min_dynamic_safe_zone[axis], target_min_limit[axis], delta_limit, delta_limit);
						max_dynamic_safe_zone[axis] = MathTools::TargetValue(max_dynamic_safe_zone[axis], target_max_limit[axis], delta_limit, delta_limit);
					}
					// restrict the pawn to the dynamic safe zone
					glm::vec2 mn = slow_safe_corners.first  + 2.0f * min_dynamic_safe_zone * safe_box.half_size;
					glm::vec2 mx = slow_safe_corners.second + 2.0f * max_dynamic_safe_zone * safe_box.half_size;

					box2 dynamic_safe_box = std::make_pair(mn, mx);
					glm::vec2 p = dynamic_safe_box.position;
					RestrictToInside(dynamic_safe_box, pawn_box, true);
					camera_box.position += (dynamic_safe_box.position - p);

					pawn_previous_position = pawn_box.position;
				}
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
		JSONTools::SetAttribute(json_entry, "fast_pawn_speed", fast_pawn_speed);
		JSONTools::SetAttribute(json_entry, "idle_pawn_speed", idle_pawn_speed);
		JSONTools::SetAttribute(json_entry, "limit_speed", limit_speed);
		JSONTools::SetAttribute(json_entry, "manual_limit_speed", manual_limit_speed);
		JSONTools::SetAttribute(json_entry, "recenter_limit_speed", recenter_limit_speed);
		JSONTools::SetAttribute(json_entry, "idle_delay", idle_delay);
		return true;
	}

	bool AutoRecenterToPlayerCameraComponent::SerializeFromJSON(nlohmann::json const& json_entry)
	{
		if (!CameraComponent::SerializeFromJSON(json_entry))
			return false;
		JSONTools::GetAttribute(json_entry, "fast_pawn_speed", fast_pawn_speed);
		JSONTools::GetAttribute(json_entry, "idle_pawn_speed", idle_pawn_speed);
		JSONTools::GetAttribute(json_entry, "limit_speed", limit_speed);
		JSONTools::GetAttribute(json_entry, "manual_limit_speed", manual_limit_speed);
		JSONTools::GetAttribute(json_entry, "recenter_limit_speed", recenter_limit_speed);
		JSONTools::GetAttribute(json_entry, "idle_delay", idle_delay);
		return true;
	}

}; // namespace chaos
