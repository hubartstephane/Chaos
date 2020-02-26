#include <death/Camera.h>
#include <death/CameraComponent.h>
#include <death/ComponentOwner.h>
#include <death/LevelInstance.h>

namespace death
{

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

	Camera::Camera(LevelInstance * in_level_instance) :
		level_instance(in_level_instance)
	{
		assert(in_level_instance != nullptr);
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
	
	chaos::AutoCastable<Player> Camera::GetPlayer(size_t player_index)
	{
		if (level_instance == nullptr)
			return nullptr;
		return level_instance->GetPlayer(player_index);
	}
	
	chaos::AutoConstCastable<Player> Camera::GetPlayer(size_t player_index) const
	{
		if (level_instance == nullptr)
			return nullptr;
		return level_instance->GetPlayer(player_index);
	}

	chaos::AutoCastable<Game> Camera::GetGame()
	{
		if (level_instance == nullptr)
			return nullptr;
		return level_instance->GetGame();
	}

	chaos::AutoConstCastable<Game> Camera::GetGame() const
	{
		if (level_instance == nullptr)
			return nullptr;
		return level_instance->GetGame();
	}

	chaos::AutoCastable<Level> Camera::GetLevel()
	{
		if (level_instance == nullptr)
			return nullptr;
		return level_instance->GetLevel();
	}

	chaos::AutoConstCastable<Level> Camera::GetLevel() const
	{
		if (level_instance == nullptr)
			return nullptr;
		return level_instance->GetLevel();
	}

	chaos::AutoCastable<GameInstance> Camera::GetGameInstance()
	{
		if (level_instance == nullptr)
			return nullptr;
		return level_instance->GetGameInstance();
	}

	chaos::AutoConstCastable<GameInstance> Camera::GetGameInstance() const
	{
		if (level_instance == nullptr)
			return nullptr;
		return level_instance->GetGameInstance();
	}

	DEATH_IMPLEMENT_COMPONENT_OWNER(Camera, CameraComponent, Component, components, camera)

}; // namespace death
