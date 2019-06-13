#include <death/CameraComponent.h>
#include <death/Camera.h>
#include <death/Player.h>

namespace death
{

	// =================================================
	// CameraComponent
	// =================================================

	CameraComponent::CameraComponent()
	{	
	}

	void CameraComponent::OnInsertedInto(Camera * in_camera)
	{
		assert(in_camera != nullptr);
	}

	void CameraComponent::OnRemovedFrom(Camera * in_camera)
	{
		assert(in_camera != nullptr);		
	}

	// =============================================
	// FollowPlayerCameraComponent
	// =============================================

	bool FollowPlayerCameraComponent::DoTick(double delta_time)
	{
		// get the wanted player
		Player * player = camera->GetPlayer(player_index);
		if (player == nullptr)
			return true;

		// get camera, cannot continue if it is empty
		chaos::box2 camera_box = camera->GetCameraBox();
		if (IsEmpty(camera_box))
			return true;

		// keep player inside camera safe zone
		chaos::box2 player_box = player->GetPlayerBox();
		if (!IsEmpty(player_box))
		{
			chaos::box2 safe_camera = camera_box;
			safe_camera.half_size *= camera->GetSafeZone();

			if (chaos::RestrictToInside(safe_camera, player_box, true)) // apply the safe_zone displacement to the real camera
				camera_box.position = safe_camera.position;
		}

		// try to keep the camera in the world
		chaos::box2 world = camera->GetLevelInstance()->GetBoundingBox();
		if (!IsEmpty(world))
			chaos::RestrictToInside(world, camera_box, false);

		// apply camera changes
		camera->SetCameraBox(camera_box);
		
		return true;
	}

}; // namespace death
