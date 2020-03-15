#include <death/FreeCameraComponent.h>
#include <death/Camera.h>
#include <death/Player.h>

namespace death
{
	// =============================================
	// FreeCameraComponent
	// =============================================

	bool FreeCameraComponent::DoTick(float delta_time)
	{
		CameraComponent::DoTick(delta_time);

		// get the wanted player
		Player * player = camera->GetPlayer(player_index);
		if (player == nullptr)
			return true;

		// get camera, cannot continue if it is empty
		chaos::box2 camera_box = camera->GetCameraBox(false); // no modifier
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
		// XXX : -multiply by 'half_size' => the greater the view is, the faster we go
		//       -screen Y coordinate and gamepad are inverted
		camera_box.position += glm::vec2(1.0f, -1.0f) * left_stick_position * delta_time * camera_box.half_size * 3.0f;

		// try to keep the camera in the world
		chaos::box2 world = camera->GetLevelInstance()->GetBoundingBox();
		if (!IsGeometryEmpty(world))
			chaos::RestrictToInside(world, camera_box, false);

		// apply camera changes
		camera->SetCameraBox(camera_box);

		return true;
	}

	chaos::box2 FreeCameraComponent::ApplyModifier(chaos::box2 const & src) const
	{
		chaos::box2 result = CameraComponent::ApplyModifier(src);
		result.half_size *= zoom_value;
		return result;
	}

}; // namespace death
