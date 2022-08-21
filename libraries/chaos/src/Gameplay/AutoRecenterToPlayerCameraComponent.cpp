#include "chaos/Chaos.h"

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
	//
	// min_dynamic_safe_zone : is the signed distance from     slow_safe_box.bottom_left (unit is the whole safe_zone size)
	// max_dynamic_safe_zone : is the signed distance from     slow_safe_box.top_right   (unit is the whole safe_zone size)
	//

	bool AutoRecenterToPlayerCameraComponent::DoTick(float delta_time)
	{
		CameraComponent::DoTick(delta_time);

		// 1.1 just to be sure that the space between the 2 limits is greater that pawn size
		float constexpr PAWNBOX_INCREASE_FACTOR = 1.1f;

		// gets the player
		Player const* player = GetPlayer(player_index);
		if (player == nullptr)
			return true;
		// gets the wanted target
		box2 pawn_box = GetTargetBox();
		if (IsGeometryEmpty(pawn_box))
			return true;
		// get camera, cannot continue if it is empty
		box2 camera_box = camera->GetCameraBox(false);
		if (IsGeometryEmpty(camera_box))
			return true;

		box2 world = GetLevelInstance()->GetBoundingBox();

		// keep pawn inside camera safe zone
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

			// by default, considere the moving speed
			glm::vec2 min_speed = { limit_speed, limit_speed };
			glm::vec2 max_speed = { limit_speed, limit_speed };

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
					min_speed[axis] = max_speed[axis] = manual_limit_speed;
					target_min_limit[axis] = GetNormalizedLimitValue(slow_safe_corners.first[axis], safe_corners.first[axis], safe_box.half_size[axis]);
					target_max_limit[axis] = GetNormalizedLimitValue(slow_safe_corners.second[axis], safe_corners.first[axis], safe_box.half_size[axis]) + pawn_box_ratio[axis];
				}
				// camera manually displaced to the left/down
				else if (right_stick[axis] < 0.0f)
				{
					idle_timer = 0.0f;
					min_speed[axis] = max_speed[axis] = manual_limit_speed;
					target_min_limit[axis] = GetNormalizedLimitValue(slow_safe_corners.first[axis], safe_corners.second[axis], safe_box.half_size[axis]) - pawn_box_ratio[axis];
					target_max_limit[axis] = GetNormalizedLimitValue(slow_safe_corners.second[axis], safe_corners.second[axis], safe_box.half_size[axis]);
				}
				else if (pawn_speed <= idle_pawn_speed)
				{
					float other_axis_pawn_speed = std::abs(pawn_previous_position[1 - axis] - pawn_box.position[1 - axis]) / delta_time;

					if (other_axis_pawn_speed <= idle_pawn_speed) // if pawn idle in both axis, do not move the camera
					{                                             // ie. if pawn is moving in a single axis, the camera recenter itself on the other axis
						target_min_limit[axis] = min_dynamic_safe_zone[axis];
						target_max_limit[axis] = max_dynamic_safe_zone[axis];
					}
				}
				else
				{
					// nop: target is already 0 
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
				// XXX : we do not want both axis to be interpolated as the same speed
				//       because depending to the position of the pawn on the camera, one axis interpolation would ends before the other
				//       instead, we want both interpolation to ends at the same time
				//       so the farest the pawn is for one axis, the faster the interpolation
				//
				//       BAD interpolation.
				//         -same speed for both axis
				//         -but more distance to go along X axis than Y
				//
				//       X initial position on screen
				//        .  
				//         .
				//          ........X final position on screen
				//      
				//       GOOD interpolation
				//         -no the same distance along X & Y axis
				//         -speed is not the same along theses axis so that we have the impression the pawn movement is linear
				//
				//       X initial position on screen
				//        ...  
				//           ...
				//              ..X final position on screen   
				{
					float d1 = std::abs(min_dynamic_safe_zone[0] - target_min_limit[0]);
					float d2 = std::abs(min_dynamic_safe_zone[1] - target_min_limit[1]);
					min_speed[0] = recenter_limit_speed * d1 / (d1 + d2);
					min_speed[1] = recenter_limit_speed * d2 / (d1 + d2);
				}
				{
					float d1 = std::abs(max_dynamic_safe_zone[0] - target_max_limit[0]);
					float d2 = std::abs(max_dynamic_safe_zone[1] - target_max_limit[1]);
					max_speed[0] = recenter_limit_speed * d1 / (d1 + d2);
					max_speed[1] = recenter_limit_speed * d2 / (d1 + d2);
				}
			}

			// compute dynamic limit by interpolation
			for (size_t axis : {0, 1})
			{
				float min_delta = min_speed[axis] * delta_time;
				float max_delta = max_speed[axis] * delta_time;
				min_dynamic_safe_zone[axis] = MathTools::TargetValue(min_dynamic_safe_zone[axis], target_min_limit[axis], min_delta, min_delta);
				max_dynamic_safe_zone[axis] = MathTools::TargetValue(max_dynamic_safe_zone[axis], target_max_limit[axis], max_delta, max_delta);
			}
			// restrict the pawn to the dynamic safe zone
			glm::vec2 mn = slow_safe_corners.first + 2.0f * min_dynamic_safe_zone * safe_box.half_size;
			glm::vec2 mx = slow_safe_corners.second + 2.0f * max_dynamic_safe_zone * safe_box.half_size;

			box2 dynamic_safe_box = std::make_pair(mn, mx);
			glm::vec2 p = dynamic_safe_box.position;
			RestrictToInside(dynamic_safe_box, pawn_box, true);
			camera_box.position += (dynamic_safe_box.position - p);

			pawn_previous_position = pawn_box.position;
		}

		// try to keep the camera in the world
		if (!IsGeometryEmpty(world))
			RestrictToInside(world, camera_box, false);
		// apply camera changes
		camera->SetCameraBox(camera_box);

		return true;
	}

	box2 AutoRecenterToPlayerCameraComponent::GetTargetBox() const
	{
		// gets the player
		Player const* player = GetPlayer(player_index);
		if (player == nullptr)
			return {};
		// gets the pawn
		PlayerPawn const* player_pawn = player->GetPawn();
		if (player_pawn == nullptr)
			return {};
		// gets the geometry
		return player_pawn->GetBoundingBox();
	}

	bool AutoRecenterToPlayerCameraComponent::SerializeIntoJSON(nlohmann::json& json) const
	{
		if (!CameraComponent::SerializeIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "fast_pawn_speed", fast_pawn_speed);
		JSONTools::SetAttribute(json, "idle_pawn_speed", idle_pawn_speed);
		JSONTools::SetAttribute(json, "limit_speed", limit_speed);
		JSONTools::SetAttribute(json, "manual_limit_speed", manual_limit_speed);
		JSONTools::SetAttribute(json, "recenter_limit_speed", recenter_limit_speed);
		JSONTools::SetAttribute(json, "idle_delay", idle_delay);
		return true;
	}

	bool AutoRecenterToPlayerCameraComponent::SerializeFromJSON(nlohmann::json const& json)
	{
		if (!CameraComponent::SerializeFromJSON(json))
			return false;
		JSONTools::GetAttribute(json, "fast_pawn_speed", fast_pawn_speed);
		JSONTools::GetAttribute(json, "idle_pawn_speed", idle_pawn_speed);
		JSONTools::GetAttribute(json, "limit_speed", limit_speed);
		JSONTools::GetAttribute(json, "manual_limit_speed", manual_limit_speed);
		JSONTools::GetAttribute(json, "recenter_limit_speed", recenter_limit_speed);
		JSONTools::GetAttribute(json, "idle_delay", idle_delay);
		return true;
	}

}; // namespace chaos
