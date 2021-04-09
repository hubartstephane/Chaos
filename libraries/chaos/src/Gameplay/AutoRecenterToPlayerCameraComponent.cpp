#include <chaos/Chaos.h>

namespace chaos
{

	// =============================================
	// AutoRecenterToPlayerCameraComponent
	// =============================================

	bool AutoRecenterToPlayerCameraComponent::DoTick(float delta_time)
	{
		CameraComponent::DoTick(delta_time);

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

	bool AutoRecenterToPlayerCameraComponent::SerializeIntoJSON(nlohmann::json& json_entry) const
	{
		if (!CameraComponent::SerializeIntoJSON(json_entry))
			return false;
		JSONTools::SetAttribute(json_entry, "recenter_speed", recenter_speed);
		JSONTools::SetAttribute(json_entry, "idle_delay", idle_delay);
		return true;
	}

	bool AutoRecenterToPlayerCameraComponent::SerializeFromJSON(nlohmann::json const& json_entry)
	{
		if (!CameraComponent::SerializeFromJSON(json_entry))
			return false;
		JSONTools::GetAttribute(json_entry, "recenter_speed", recenter_speed);
		JSONTools::GetAttribute(json_entry, "idle_delay", idle_delay);
		return true;
	}

}; // namespace chaos
