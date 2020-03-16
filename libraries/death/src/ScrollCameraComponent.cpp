#include <death/ScrollCameraComponent.h>
#include <death/Camera.h>
#include <death/Player.h>

namespace death
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
		chaos::box2 camera_box = camera->GetCameraBox(false);
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
		chaos::box2 world = level_instance->GetBoundingBox();

		// step 1 : scroll all players. Compute all players bounding box. Move the camera according the players
		chaos::box2 all_players_box;

		size_t player_count = camera->GetPlayerCount();
		for (size_t i = 0; i < player_count; ++i)
		{
			// get the PLAYER 
			Player* player = camera->GetPlayer(i);
			if (player == nullptr)
				continue;
			// get the player box
			chaos::box2 player_box = player->GetPlayerBox();
			if (IsGeometryEmpty(player_box))
				continue;
			// scroll the player and keep it into world
			player_box.position[axis_index] += scroll_displacement;
			if (!IsGeometryEmpty(world))
				chaos::RestrictToInside(world, player_box, false);
			player->SetPlayerBox(player_box);

			// the camera follows the player in X & Y direction
			chaos::box2 safe_camera = camera_box;
			safe_camera.half_size *= camera->GetSafeZone();
			chaos::RestrictToInside(safe_camera, player_box, true);
			camera_box.position = safe_camera.position;
			// ensure the camera has not been modified along the scroll direction
			camera_box.position[axis_index] = camera_position_scroll_axis;
			// the whole player bounding box
			all_players_box = all_players_box | player_box;
		}

		// compute the max displacement along the other index
		float min_player_box_other_axis = all_players_box.position[1 - axis_index] - all_players_box.half_size[1 - axis_index];
		float max_player_box_other_axis = all_players_box.position[1 - axis_index] + all_players_box.half_size[1 - axis_index];

		// the camera position must stay between theses 2 values 
		float p1_other_axis = min_player_box_other_axis + camera_box.half_size[1 - axis_index];
		float p2_other_axis = max_player_box_other_axis - camera_box.half_size[1 - axis_index];		


		if (p2_other_axis - p1_other_axis > 2.0f * camera_box.half_size[1 - axis_index]) // range too big : dont move
			camera_box.position[1 - axis_index] = camera_position_scroll_other_axis;
		else if (camera_box.position[1 - axis_index] > p1_other_axis)
			camera_box.position[1 - axis_index] = p1_other_axis;
		else if (camera_box.position[1 - axis_index] < p2_other_axis)
			camera_box.position[1 - axis_index] = p2_other_axis;

		// restrict camera to world		
		if (!IsGeometryEmpty(world))
			chaos::RestrictToInside(world, camera_box, false);

		// apply the compute result
		camera->SetCameraBox(camera_box);

		// step 2 : make all player stay inside the camera (maybe 2 players want to go in opposite direction)
		for (size_t i = 0; i < player_count; ++i)
		{
			// get the PLAYER 
			Player* player = camera->GetPlayer(i);
			if (player == nullptr)
				continue;
			// get the player box
			chaos::box2 player_box = player->GetPlayerBox();
			if (IsGeometryEmpty(player_box))
				continue;
			// keep player inside camera
			if (chaos::RestrictToInside(camera_box, player_box, false))
				player->SetPlayerBox(player_box);
		}

		return true;
	}

}; // namespace death
