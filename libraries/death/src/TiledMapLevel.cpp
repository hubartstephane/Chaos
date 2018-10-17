#include <death/TiledMapLevel.h>

namespace death
{
	TiledMapLevel::TiledMapLevel(chaos::TiledMap::Map * in_tiled_map):
		tiled_map(in_tiled_map)
	{
		assert(in_tiled_map != nullptr);
	}


	GameLevelInstance * TiledMapLevel::DoCreateLevelInstance()
	{
		return new TiledMapLevelInstance;
	}

}; // namespace death

