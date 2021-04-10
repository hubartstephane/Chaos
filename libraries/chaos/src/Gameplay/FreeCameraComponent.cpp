#include <chaos/Chaos.h>

namespace chaos
{
	// =============================================
	// FreeCameraComponent
	// =============================================

	bool FreeCameraComponent::DoTick(float delta_time)
	{
		CameraComponent::DoTick(delta_time);

		// get the wanted player
		Player * player = GetPlayer(player_index);
		if (player == nullptr)
			return true;

		// get camera, cannot continue if it is empty
		box2 camera_box = camera->GetCameraBox(false); // no modifier
		if (IsGeometryEmpty(camera_box))
			return true;

		// get the left stick
		glm::vec2 left_stick_position = player->GetLeftStickPosition();

		// prepare to zoom
		float zoom_in  = player->GetLeftTrigger();
		float zoom_out = player->GetRightTrigger();

		if ((zoom_in == 0.0f) ^ (zoom_out == 0.0f))
		{
			if (zoom_in > 0.0f)
				zoom_value -= 1.0f * zoom_in * delta_time;				
			else if (zoom_out > 0.0f)
				zoom_value += 1.0f * zoom_value * delta_time;
		}

		zoom_value = std::clamp(zoom_value, 1.0f, 4.0f);

		// update camera position
		// XXX : multiply by 'half_size' => the greater the view is, the faster we go
		camera_box.position += left_stick_position * delta_time * camera_box.half_size * 3.0f;

		// try to keep the camera in the world
		box2 world = camera->GetLevelInstance()->GetBoundingBox();
		if (!IsGeometryEmpty(world))
			RestrictToInside(world, camera_box, false);

		// apply camera changes
		camera->SetCameraBox(camera_box);

		return true;
	}

	box2 FreeCameraComponent::ApplyModifier(box2 const & src) const
	{
		box2 result = CameraComponent::ApplyModifier(src);
		result.half_size *= zoom_value;
		return result;
	}

}; // namespace chaos
