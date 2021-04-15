#include <chaos/Chaos.h>

namespace chaos
{

	// =============================================
	// FollowPlayerCameraComponent
	// =============================================

	bool FollowPlayerCameraComponent::DoTick(float delta_time)
	{
		CameraComponent::DoTick(delta_time);

		// get the wanted player
		Player * player = GetPlayer(player_index);
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
				box2 safe_camera = CameraTools::GetSafeCameraBox(camera_box, camera->GetSafeZone());
				glm::vec2 p = safe_camera.position;
				if (RestrictToInside(safe_camera, pawn_box, true)) // apply the safe_zone displacement to the real camera
					camera_box.position += (safe_camera.position - p);
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

}; // namespace chaos
