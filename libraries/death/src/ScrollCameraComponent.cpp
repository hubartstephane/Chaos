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

		// compute scroll for both camera and player
		float scroll_displacement = scroll_speed * delta_time;

		// scroll the camera in given direction and keep the wanted value
		camera_box.position[(size_t)axis] += scroll_displacement;

		float camera_position = camera_box.position[(size_t)axis];

		// get the world
		chaos::box2 world = camera->GetLevelInstance()->GetBoundingBox();

		// step 1 : scroll all players and camera follows players in axis not concerned by the scroll
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

			// scroll the player
			player_box.position[(size_t)axis] += scroll_displacement;
			if (!IsGeometryEmpty(world))
				chaos::RestrictToInside(world, player_box, false);
			player->SetPlayerBox(player_box);

			// the camera follows the player in X & Y direction
			chaos::box2 safe_camera = camera_box;
			safe_camera.half_size *= camera->GetSafeZone();
			chaos::RestrictToInside(safe_camera, player_box, true);
			camera_box.position = safe_camera.position;
			
			// ensure the camera has not been modified along the scroll direction
			camera_box.position[(size_t)axis] = camera_position;
		}

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
