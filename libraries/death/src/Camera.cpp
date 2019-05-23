#include <death/Camera.h>

#include <chaos/ParticleDefault.h>

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

}; // namespace death
