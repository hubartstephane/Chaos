#include <chaos/Chaos.h>

#include <death/ShakeCameraComponent.h>
#include <death/Camera.h>
#include <death/Player.h>

namespace death
{
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

}; // namespace death
