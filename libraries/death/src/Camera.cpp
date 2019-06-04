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
	
	DEATH_IMPLEMENT_COMPONENT_OWNER(Camera, CameraComponent, Component, components, camera)


}; // namespace death
