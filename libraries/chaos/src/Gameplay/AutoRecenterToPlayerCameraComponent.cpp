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



	float MN = 0.0f;
	float MX = 0.0f;
	float SPEED = 0.0f;

	bool AutoRecenterToPlayerCameraComponent::DoTick(float delta_time)
	{
		CameraComponent::DoTick(delta_time);

		fast_pawn_speed = 300.0f;
		idle_pawn_speed = 50.0f;
		//slow_safe_zone = { {0.0f, 0.0f}, {1.0f, 1.0f} };




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

					glm::vec2 target_min_limit = { 0.0f, 0.0f }; // limit expressed with safe_zone as unit
					glm::vec2 target_max_limit = { 0.0f, 0.0f }; // 0 for limits on the slow_safe_zone

					std::pair<glm::vec2, glm::vec2> safe_corners = GetBoxCorners(safe_box);
					std::pair<glm::vec2, glm::vec2> slow_safe_corners = GetBoxCorners(slow_safe_box);

					glm::vec2 pawn_box_ratio = PAWNBOX_INCREASE_FACTOR * pawn_box.half_size / safe_box.half_size;

					// analyse the speed
					glm::bvec2 fast_pawn = { false, false };
					glm::bvec2 idle_pawn = { false, false };
					for (size_t axis : {0, 1})
					{
						float pawn_speed = std::abs(pawn_previous_position[axis] - pawn_box.position[axis]) / delta_time;

						if (axis == 0)
							SPEED = pawn_speed;

						if (pawn_speed >= fast_pawn_speed)
							fast_pawn[axis] = true;
						else if (pawn_speed <= idle_pawn_speed)
							idle_pawn[axis] = true;
					}

					glm::vec2 wanted_limit_speed = { limit_speed, limit_speed }; // by default, considere the moving speed

					// idle
					if (idle_pawn[0] && idle_pawn[1])
					{
						for (size_t axis : {0, 1})
						{
							// camera manually displaced to the right/up
							if (right_stick[axis] > 0.0f)
							{
								idle_timer = 0.0f;
								wanted_limit_speed = { manual_camera_speed, manual_camera_speed };
								target_min_limit[axis] = GetNormalizedLimitValue(slow_safe_corners.first[axis], safe_corners.first[axis], safe_box.half_size[axis]);
								target_max_limit[axis] = GetNormalizedLimitValue(slow_safe_corners.second[axis], safe_corners.first[axis], safe_box.half_size[axis]) + pawn_box_ratio[axis];
							}
							// camera manually displaced to the left/down
							else if (right_stick[axis] < 0.0f)
							{
								idle_timer = 0.0f;
								wanted_limit_speed = { manual_camera_speed, manual_camera_speed };
								target_min_limit[axis] = GetNormalizedLimitValue(slow_safe_corners.first[axis], safe_corners.second[axis], safe_box.half_size[axis]) - pawn_box_ratio[axis];
								target_max_limit[axis] = GetNormalizedLimitValue(slow_safe_corners.second[axis], safe_corners.second[axis], safe_box.half_size[axis]);
							}
							// target is the current position
							else
							{
								target_min_limit[axis] = min_dynamic_safe_zone[axis];
								target_max_limit[axis] = max_dynamic_safe_zone[axis];
							}
						}

						// autocenter
						if (right_stick == glm::vec2(0.0f, 0.0f))
						{
							idle_timer = std::min(idle_timer + delta_time, idle_delay);
							if (idle_timer >= idle_delay)
							{
								for (size_t axis : {0, 1})
								{
									target_min_limit[axis] = GetNormalizedLimitValue(slow_safe_corners.first[axis], slow_safe_box.position[axis] - pawn_box.half_size[axis], safe_box.half_size[axis]);
									target_max_limit[axis] = GetNormalizedLimitValue(slow_safe_corners.second[axis], slow_safe_box.position[axis] + pawn_box.half_size[axis], safe_box.half_size[axis]);
								}
							}
						}
					}
					else
					{
						idle_timer = 0.0f;
						// fast
						if (fast_pawn[0] || fast_pawn[1])
						{
							for (size_t axis : {0, 1})
							{
								if (fast_pawn[axis]) //  other axis are considered like slow displacement
								{
									// pawn going right/up
									if (pawn_previous_position[axis] < pawn_box.position[axis])
									{
										target_min_limit[axis] = GetNormalizedLimitValue(slow_safe_corners.first[axis],  safe_corners.first[axis], safe_box.half_size[axis]);
										target_max_limit[axis] = GetNormalizedLimitValue(slow_safe_corners.second[axis], safe_corners.first[axis], safe_box.half_size[axis]) + pawn_box_ratio[axis];
									}
									// pawn going left/down
									else if (pawn_previous_position[axis] > pawn_box.position[axis])
									{
										target_min_limit[axis] = GetNormalizedLimitValue(slow_safe_corners.first[axis], safe_corners.second[axis], safe_box.half_size[axis]) - pawn_box_ratio[axis];
										target_max_limit[axis] = GetNormalizedLimitValue(slow_safe_corners.second[axis], safe_corners.second[axis], safe_box.half_size[axis]);
									}
								}
							}
						}
						// slow
						else
						{
							// nop
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

					MN = min_dynamic_safe_zone.x;
					MX = max_dynamic_safe_zone.x;

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









#if 0

	glm::vec2 safe_zone = camera->GetSafeZone();

	// initialize previous frame position if not initialized
	if (isnan(pawn_previous_position[0]) || isnan(pawn_previous_position[1]))
		pawn_previous_position = pawn_box.position;

	// move the camera with the right stick
	glm::vec2 right_stick = player->GetRightStickPosition();

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

		if (pawn_speed > fast_pawn_limit || right_stick[axis] != 0.0f)
		{
			idle_timer = 0.0f;

			if (PAWNBOX_INCREASE_FACTOR * pawn_box.half_size[axis] < camera_box.half_size[axis])
			{
				// XXX : order of theses tests is important : pawn movement first, manual camera offset thereafter

				// pawn going right/up
				if (pawn_previous_position[axis] < pawn_box.position[axis])
				{
					min_dynamic_safe_zone[axis] = std::max(min_dynamic_safe_zone[axis] - safe_zone_speed * delta_time, 0.0f);
					max_dynamic_safe_zone[axis] = std::min(max_dynamic_safe_zone[axis] + safe_zone_speed * delta_time, 1.0f - pawn_box_ratio[axis]);

				}
				// pawn going left/down
				else if (pawn_previous_position[axis] > pawn_box.position[axis])
				{
					min_dynamic_safe_zone[axis] = std::min(min_dynamic_safe_zone[axis] + safe_zone_speed * delta_time, 1.0f - pawn_box_ratio[axis]);
					max_dynamic_safe_zone[axis] = std::max(max_dynamic_safe_zone[axis] - safe_zone_speed * delta_time, 0.0f);
				}
				// camera manually displaced to the right/up
				else if (right_stick[axis] > 0.0f)
				{
					min_dynamic_safe_zone[axis] = std::max(min_dynamic_safe_zone[axis] - manual_camera_speed * delta_time, 0.0f);
					max_dynamic_safe_zone[axis] = std::min(max_dynamic_safe_zone[axis] + manual_camera_speed * delta_time, 1.0f - pawn_box_ratio[axis]);
				}
				// camera manually displaced to the left/down
				else if (right_stick[axis] < 0.0f)
				{
					min_dynamic_safe_zone[axis] = std::min(min_dynamic_safe_zone[axis] + manual_camera_speed * delta_time, 1.0f - pawn_box_ratio[axis]);
					max_dynamic_safe_zone[axis] = std::max(max_dynamic_safe_zone[axis] - manual_camera_speed * delta_time, 0.0f);
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
		if (right_stick.x == 0.0f && right_stick.y == 0.0f) // no auto center if player want to move the camera
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








#endif





















	bool AutoRecenterToPlayerCameraComponent::SerializeIntoJSON(nlohmann::json& json_entry) const
	{
		if (!CameraComponent::SerializeIntoJSON(json_entry))
			return false;
		JSONTools::SetAttribute(json_entry, "fast_pawn_speed", fast_pawn_speed);
		JSONTools::SetAttribute(json_entry, "idle_pawn_speed", idle_pawn_speed);
		JSONTools::SetAttribute(json_entry, "limit_speed", limit_speed);
		JSONTools::SetAttribute(json_entry, "manual_camera_speed", manual_camera_speed);
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
		JSONTools::GetAttribute(json_entry, "manual_camera_speed", manual_camera_speed);
		JSONTools::GetAttribute(json_entry, "idle_delay", idle_delay);
		return true;
	}

}; // namespace chaos
