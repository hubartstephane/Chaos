#include <chaos/Chaos.h>

namespace chaos
{

	CHAOS_IMPLEMENT_GAMEPLAY_GETTERS(Camera);

	// =================================================
	// CameraTransform
	// =================================================

	glm::mat4x4 CameraTransform::GetCameraTransform(chaos::obox2 const & obox)
	{
		glm::mat4x4 result;	
		result = chaos::GetRotatorMatrix(-obox.rotator) * glm::translate(glm::vec3(-obox.position.x, -obox.position.y, 0.0f));
		return result;
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

	chaos::box2 Camera::GetCameraBox(bool apply_modifiers) const 
	{
		chaos::box2 result = camera_box;

		// apply the aspect ratio
		Game const * game = GetGame();
		if (game != nullptr)
			chaos::AlterBoxToAspect(result, game->GetViewportWantedAspect(), true);

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

	chaos::obox2 Camera::GetCameraOBox(bool apply_modifiers) const
	{
		chaos::box2 box = GetCameraBox(apply_modifiers);

		chaos::obox2 result;
		result.position = box.position;
		result.half_size = box.half_size;
		result.rotator = 0.0f;
		return result;
	}

	
	bool Camera::SerializeIntoJSON(nlohmann::json& json_entry) const
	{
		if (!chaos::JSONSerializable::SerializeIntoJSON(json_entry))
			return false;
		chaos::JSONTools::SetAttribute(json_entry, "CAMERA_BOX", GetCameraBox(false));
		return true;
	}
	
	bool Camera::SerializeFromJSON(nlohmann::json const& json_entry)
	{
		if (!chaos::JSONSerializable::SerializeFromJSON(json_entry))
			return false;
		chaos::box2 b;
		if (chaos::JSONTools::GetAttribute(json_entry, "CAMERA_BOX", b))
			SetCameraBox(b);
		return true;
	}

	CHAOS_IMPLEMENT_COMPONENT_OWNER(Camera, CameraComponent, Component, components, camera)

}; // namespace chaos
