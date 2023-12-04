#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	// =============================================
	// ScrollCameraComponent
	// =============================================

	bool ScrollCameraComponent::DoTick(float delta_time)
	{
		CameraComponent::DoTick(delta_time);
		// get the level instance
		LevelInstance* level_instance = camera->GetLevelInstance();
		if (level_instance == nullptr)
			return true;
		// get the camera box without effects
		box2 camera_box = camera->GetCameraBox(false);
		if (IsGeometryEmpty(camera_box))
			return true;

		size_t axis_index = (size_t)axis;

		// compute scroll for both camera and player
		float scroll_displacement = scroll_speed * delta_time;

		// scroll the camera in given direction and keep the wanted value	
		float camera_position_scroll_axis = camera_box.position[axis_index] + scroll_displacement;
		camera_box.position[axis_index] = camera_position_scroll_axis;

		float camera_position_scroll_other_axis = camera_box.position[1 - axis_index];

		// get the world
		box2 world = level_instance->GetBoundingBox();

		// step 1 : scroll all pawns. Compute all players bounding box. Move the camera according the players
		box2 all_pawns_box;

		size_t player_count = camera->GetPlayerCount();
		for (size_t i = 0; i < player_count; ++i)
		{
			// get the player pawn 
			PlayerPawn * player_pawn = camera->GetPlayerPawn(i);
			if (player_pawn == nullptr)
				continue;
			// get the player box
			box2 pawn_box = player_pawn->GetBoundingBox();
			if (IsGeometryEmpty(pawn_box))
				continue;
			// scroll the player and keep it into world
			pawn_box.position[axis_index] += scroll_displacement;
			if (!IsGeometryEmpty(world))
				RestrictToInside(world, pawn_box, false);
			player_pawn->SetBoundingBox(pawn_box);

			// the camera follows the player in X & Y direction
			box2 safe_camera = CameraTools::GetSafeCameraBox(camera_box, camera->GetSafeZone());
			glm::vec2 p = safe_camera.position;
			if (RestrictToInside(safe_camera, pawn_box, true)) // apply the safe_zone displacement to the real camera
				camera_box.position += (safe_camera.position - p);
			// ensure the camera has not been modified along the scroll direction
			camera_box.position[axis_index] = camera_position_scroll_axis;
			// the whole player bounding box
			all_pawns_box = all_pawns_box | pawn_box;
		}

		// compute the max displacement along the other index
		float min_pawn_box_other_axis = all_pawns_box.position[1 - axis_index] - all_pawns_box.half_size[1 - axis_index];
		float max_pawn_box_other_axis = all_pawns_box.position[1 - axis_index] + all_pawns_box.half_size[1 - axis_index];
		// the camera position must stay between theses 2 values 
		float p1_other_axis = min_pawn_box_other_axis + camera_box.half_size[1 - axis_index];
		float p2_other_axis = max_pawn_box_other_axis - camera_box.half_size[1 - axis_index];

		if (p2_other_axis - p1_other_axis > 2.0f * camera_box.half_size[1 - axis_index]) // range too big : dont move
			camera_box.position[1 - axis_index] = camera_position_scroll_other_axis;
		else if (camera_box.position[1 - axis_index] > p1_other_axis)
			camera_box.position[1 - axis_index] = p1_other_axis;
		else if (camera_box.position[1 - axis_index] < p2_other_axis)
			camera_box.position[1 - axis_index] = p2_other_axis;

		// restrict camera to world		
		if (!IsGeometryEmpty(world))
			RestrictToInside(world, camera_box, false);
		// apply the compute result
		camera->SetCameraBox(camera_box);
		// step 2 : make all pawns stay inside the camera (maybe 2 players want to go in opposite direction)
		for (size_t i = 0; i < player_count; ++i)
		{
			// get the player pawn 
			PlayerPawn * player_pawn = camera->GetPlayerPawn(i);
			if (player_pawn == nullptr)
				continue;
			// get the pawn box
			box2 pawn_box = player_pawn->GetBoundingBox();
			if (IsGeometryEmpty(pawn_box))
				continue;
			// keep player inside camera
			if (RestrictToInside(camera_box, pawn_box, false))
				player_pawn->SetBoundingBox(pawn_box);
		}
		return true;
	}

	bool ScrollCameraComponent::SerializeIntoJSON(nlohmann::json * json) const
	{
		if (!CameraComponent::SerializeIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "scroll_speed", scroll_speed);
		JSONTools::SetAttribute(json, "axis", axis);
		return true;
	}

	bool ScrollCameraComponent::SerializeFromJSON(JSONReadConfiguration config)
	{
		if (!CameraComponent::SerializeFromJSON(config))
			return false;
		JSONTools::GetAttribute(config, "scroll_speed", scroll_speed);
		JSONTools::GetAttribute(config, "axis", axis);
		return true;
	}

}; // namespace chaos
