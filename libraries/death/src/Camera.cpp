#include <death/Camera.h>
#include <death/CameraComponent.h>
#include <death/ComponentOwner.h>

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
	
	bool Camera::DoTick(double delta_time)
	{
		// tick all components
		size_t count = components.size();
		for (size_t i = 0; i < count; ++i)
			components[i]->Tick(delta_time);
		return true;
	}

	DEATH_IMPLEMENT_COMPONENT_OWNER(Camera, CameraComponent, Component, components, camera)

}; // namespace death
