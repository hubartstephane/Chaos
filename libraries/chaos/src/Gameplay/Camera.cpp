#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	CHAOS_IMPLEMENT_GAMEPLAY_GETTERS(Camera);

	// =================================================
	// CameraTools
	// =================================================

	glm::mat4x4 CameraTools::GetCameraTransform(obox2 const & ob)
	{
		glm::mat4x4 result;
		result = GetRotationMatrix(-ob.rotation) * glm::translate(glm::vec3(-ob.position.x, -ob.position.y, 0.0f));
		return result;
	}

	glm::mat4x4 CameraTools::GetProjectionMatrix(obox2 const& ob)
	{
		glm::mat4x4 result;
		result = glm::scale(glm::vec3(1.0f / ob.half_size.x, 1.0f / ob.half_size.y, 1.0f));
		return result;
	}

	box2 CameraTools::GetSafeCameraBox(box2 const & camera_box, SafeZone const & safe_zone)
	{
		box_corners corners = GetBoxCorners(camera_box);
		return box2(
			corners.min + safe_zone.min * (corners.max - corners.min),
			corners.min + safe_zone.max * (corners.max - corners.min)
		);
	}

	// =================================================
	// Camera
	// =================================================

	bool Camera::Initialize(LevelInstance * in_level_instance)
	{
		// ensure valid arguments and not already initialized
		assert(in_level_instance != nullptr);
		assert(level_instance == nullptr);
		level_instance = in_level_instance;
		return true;
	}

	bool Camera::DoTick(float delta_time)
	{
		// tick all components
		size_t count = components.size();
		for (size_t i = 0; i < count; ++i)
			components[i]->Tick(delta_time);
		return true;
	}

	box2 Camera::GetCameraBox(bool apply_modifiers) const
	{
		box2 result = camera_box;

		// apply the aspect ratio
		Game const * game = GetGame();
		if (game != nullptr)
			SetBoxAspect(result, game->GetViewportWantedAspect(), SetBoxAspectMethod::PREFER_UPDATE_WIDTH);

		// apply the modifiers
		if (apply_modifiers)
		{
			size_t count = components.size();
			for (size_t i = 0; i < count; ++i)
			{
				CameraComponent * camera_component = components[i].get();
				if (camera_component == nullptr)
					continue;
				result = camera_component->ApplyModifier(result);
			}
		}
		return result;
	}

	obox2 Camera::GetCameraOBox(bool apply_modifiers) const
	{
		box2 camera_box = GetCameraBox(apply_modifiers);

		obox2 result;
		result.position = camera_box.position;
		result.half_size = camera_box.half_size;
		result.rotation = 0.0f;
		return result;
	}

	void Camera::SetSafeZone(glm::vec2 const& in_safe_zone)
	{
		SetSafeZone(box_corners2(
			glm::vec2(0.5f, 0.5f) - in_safe_zone * 0.5f,
			glm::vec2(0.5f, 0.5f) + in_safe_zone * 0.5f
		));
	}

	box2 Camera::GetSafeCameraBox(bool apply_modifiers) const
	{
		return CameraTools::GetSafeCameraBox(GetCameraBox(apply_modifiers), safe_zone);
	}

	void Camera::SetSafeZone(SafeZone const& in_safe_zone)
	{
		assert(in_safe_zone.min.x >= 0.0f && in_safe_zone.min.x <= 1.0f);
		assert(in_safe_zone.min.y >= 0.0f && in_safe_zone.min.y <= 1.0f);
		assert(in_safe_zone.max.x >= 0.0f && in_safe_zone.max.x <= 1.0f);
		assert(in_safe_zone.max.y >= 0.0f && in_safe_zone.max.y <= 1.0f);
		assert(in_safe_zone.min.x <= in_safe_zone.max.x);
		assert(in_safe_zone.min.y <= in_safe_zone.max.y);
		safe_zone = in_safe_zone;
	}

	bool Camera::SerializeIntoJSON(nlohmann::json * json) const
	{
		if (!JSONSerializableInterface::SerializeIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "CAMERA_BOX", GetCameraBox(false));
		return true;
	}

	bool Camera::SerializeFromJSON(JSONReadConfiguration config)
	{
		if (!JSONSerializableInterface::SerializeFromJSON(config))
			return false;
		box2 b;
		if (JSONTools::GetAttribute(config, "CAMERA_BOX", b))
			SetCameraBox(b);
		return true;
	}

	CHAOS_IMPLEMENT_COMPONENT_OWNER(Camera, CameraComponent, Component, components, camera)

}; // namespace chaos
