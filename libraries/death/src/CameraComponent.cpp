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
		current_time = 0.0f;
	}

	void ShakeCameraComponent::StopModifier()
	{
		current_time = -1.0f;
	}

	chaos::box2 ShakeCameraComponent::ApplyModifier(chaos::box2 const & src) const
	{
		chaos::box2 result = src;
		if (current_time >= 0 && current_time < modifier_duration)
		{
			float damping = (use_damping)? std::cos((float)(0.5 * M_PI) * (current_time / modifier_duration)) : 1.0f;
			float wave    = std::cos((float)(2.0 * M_PI) * (current_time / modifier_frequency));
			if (zoom_effect)
				result.half_size *= 1.0f + damping * modifier_range * (0.5f + 0.5f * wave);
			else
				result.position.x += damping * modifier_range * wave;
		}
		return result;
	}

	bool ShakeCameraComponent::DoTick(float delta_time)
	{
		CameraComponent::DoTick(delta_time);

		if (current_time >= 0.0f)
		{
			current_time += delta_time;
			if (current_time >= modifier_duration)
			{
				current_time = modifier_duration;
				StopModifier();
			}
		}
		return true;
	}

	// =============================================
	// FollowPlayerCameraComponent
	// =============================================

	bool FollowPlayerCameraComponent::DoTick(float delta_time)
	{
		CameraComponent::DoTick(delta_time);

		// get the wanted player
		Player * player = camera->GetPlayer(player_index);
		if (player == nullptr)
			return true;

		// get camera, cannot continue if it is empty
		chaos::box2 camera_box = camera->GetCameraBox(false);
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


	// =============================================
	// SoundListenerCameraComponent
	// =============================================

	bool SoundListenerCameraComponent::DoTick(float delta_time)
	{
		CameraComponent::DoTick(delta_time);

		if (camera != nullptr)
		{
			Game * game = camera->GetGame();
			if (game != nullptr)
			{
				chaos::SoundManager * sound_manager = game->GetSoundManager();
				if (sound_manager != nullptr)					
				{
					glm::vec3 position = glm::vec3(camera->GetCameraBox(false).position, 0.0f);
					glm::vec3 speed    = glm::vec3(0.0f, 0.0f, 0.0f);
					sound_manager->SetListenerPosition(position, speed);
				}
			}
		}
		return true;
	}

}; // namespace death
