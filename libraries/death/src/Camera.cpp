#include <death/Camera.h>
#include <death/CameraComponent.h>

namespace death
{

	// =================================================
	// Camera
	// =================================================

	Camera::Camera(GameLevelInstance * in_game_level_instance) :
		game_level_instance(in_game_level_instance)
	{
		assert(in_game_level_instance != nullptr);
	}
	
	size_t Camera::GetComponentCount() const
	{
		return components.size();
	}
	
	CameraComponent * Camera::GetComponent(size_t index)
	{
		if (index >= components.size())
			return nullptr;
		return components[index].get();
	}

	CameraComponent const * Camera::GetComponent(size_t index) const
	{
		if (index >= components.size())
			return nullptr;
		return components[index].get();
	}
	
	CameraComponent * Camera::FindComponent(chaos::TagType tag)
	{
		return chaos::NamedObject::FindNamedObject(components, tag);
	}
	
	CameraComponent const * Camera::FindComponent(chaos::TagType tag) const
	{
		return chaos::NamedObject::FindNamedObject(components, tag);
	}
	
	CameraComponent * Camera::FindComponent(char const * name)
	{
		return chaos::NamedObject::FindNamedObject(components, name);
	}
	
	CameraComponent const * Camera::FindComponent(char const * name) const
	{
		return chaos::NamedObject::FindNamedObject(components, name);
	}

	bool Camera::AddComponent(CameraComponent * component)
	{
		if (component == nullptr)
			return false;
		assert(component->camera == nullptr);
		components.push_back(component);
		component->camera = this;
		return true;
	}

	bool Camera::RemoveComponent(CameraComponent * component)
	{

		return true;
	}

	
	bool Camera::RemoveComponent(chaos::TagType tag)
	{
		return true;
	}
	
	bool Camera::RemoveComponent(char const * name)
	{
		return true;
	}

}; // namespace death
