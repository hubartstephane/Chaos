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

	chaos::box2 CameraComponent::ApplyModifier(chaos::box2 const & src) const
	{
		return src;
	}

	void CameraComponent::OnInsertedInto(Camera * in_camera)
	{
		assert(in_camera != nullptr);
	}

	void CameraComponent::OnRemovedFrom(Camera * in_camera)
	{
		assert(in_camera != nullptr);		
	}

	// =================================================
	// ShakeCameraComponent
	// =================================================

	void ShakeCameraComponent::RestartModifier()
	{
		current_range = modifier_range;
	}

	void ShakeCameraComponent::StopModifier()
	{
		current_range = 0.0f;
		current_time = 0.0f;
	}

	chaos::box2 ShakeCameraComponent::ApplyModifier(chaos::box2 const & src) const
	{
		chaos::box2 result = src;
		if (current_range >= 0.0f)
			result.position.x += 
				current_range * chaos::MathTools::Cos((float)(2.0 * M_PI) * (current_time / modifier_frequency));
		return result;
	}

	bool ShakeCameraComponent::DoTick(double delta_time)
	{
		if (current_range >= 0.0f)
		{
			current_range -= (float)delta_time * (modifier_range / modifier_duration);
			if (current_range <= 0.0f)
				StopModifier();
			else
				current_time += (float)delta_time;
		}
		return true;
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
		if (IsGeometryEmpty(camera_box))
			return true;

		// keep player inside camera safe zone
		chaos::box2 player_box = player->GetPlayerBox();
		if (!IsGeometryEmpty(player_box))
		{
			chaos::box2 safe_camera = camera_box;
			safe_camera.half_size *= camera->GetSafeZone();

			if (chaos::RestrictToInside(safe_camera, player_box, true)) // apply the safe_zone displacement to the real camera
				camera_box.position = safe_camera.position;
		}

		// try to keep the camera in the world
		chaos::box2 world = camera->GetLevelInstance()->GetBoundingBox();
		if (!IsGeometryEmpty(world))
			chaos::RestrictToInside(world, camera_box, false);

		// apply camera changes
		camera->SetCameraBox(camera_box);
		
		return true;
	}

	// =============================================
	// FreeCameraComponent
	// =============================================

	bool FreeCameraComponent::DoTick(double delta_time)
	{
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
			{
				zoom_value -= 1.0f * zoom_in * (float)delta_time;				
			}
			else if (zoom_out > 0.0f)
			{
				zoom_value += 1.0f * zoom_value * (float)delta_time;
			}
		}

		zoom_value = chaos::MathTools::Clamp(zoom_value, 1.0f, 4.0f);

		// update camera position
		// XXX : -multiply by 'half_size' => the greater the view is, the faster we go
		//       -screen Y coordinate and gamepad are inverted
		camera_box.position += glm::vec2(1.0f, -1.0f) * left_stick_position * (float)delta_time * camera_box.half_size * 3.0f;

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
