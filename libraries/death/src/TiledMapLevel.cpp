#include <death/TiledMapLevel.h>

namespace death
{
	TiledMapLevel::TiledMapLevel()
	{

	}

	bool TiledMapLevel::Initialize(chaos::TiledMap::Map * in_tiled_map)
	{
		assert(in_tiled_map != nullptr);

		/** already initialized ?*/
		if (tiled_map != nullptr)
			return false;
		/** copy the tile map */
		tiled_map = in_tiled_map;
		/** the real initialization */



		return true;
	}


	GameLevelInstance * TiledMapLevel::DoCreateLevelInstance()
	{
		return new TiledMapLevelInstance;
	}

}; // namespace death

