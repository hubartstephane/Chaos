#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
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

	box2 ShakeCameraComponent::ApplyModifier(box2 const & src) const
	{
		box2 result = src;
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

	bool ShakeCameraComponent::SerializeIntoJSON(nlohmann::json& json) const
	{
		if (!CameraComponent::SerializeIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "modifier_duration", modifier_duration);
		JSONTools::SetAttribute(json, "modifier_range", modifier_range);
		JSONTools::SetAttribute(json, "modifier_frequency", modifier_frequency);
		JSONTools::SetAttribute(json, "use_damping", use_damping);
		JSONTools::SetAttribute(json, "zoom_effect", zoom_effect);
		return true;
	}

	bool ShakeCameraComponent::SerializeFromJSON(nlohmann::json const& json)
	{
		if (!CameraComponent::SerializeFromJSON(json))
			return false;
		JSONTools::GetAttribute(json, "modifier_duration", modifier_duration);
		JSONTools::GetAttribute(json, "modifier_range", modifier_range);
		JSONTools::GetAttribute(json, "modifier_frequency", modifier_frequency);
		JSONTools::GetAttribute(json, "use_damping", use_damping);
		JSONTools::GetAttribute(json, "zoom_effect", zoom_effect);
		return true;
	}

}; // namespace chaos
