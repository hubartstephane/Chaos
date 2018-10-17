#pragma once

#include <death/GameLevel.h>

#include <chaos/StandardHeaders.h>
#include <chaos/GeometryFramework.h>
#include <chaos/TiledMap.h>

namespace death
{
	/** TiledMapLevel : a level described by a tiledmap */
	class TiledMapLevel : public GameLevel
	{
	public:

		/** constructor */
		TiledMapLevel();

		/** initialization from tiledmap */
		virtual bool Initialize(chaos::TiledMap::Map * in_tiled_map);

	protected:

		/** create a level instance for that level user specified function */
		virtual class GameLevelInstance * DoCreateLevelInstance() override;

	protected:

		/** the tiled map corresponding to this level */
		boost::intrusive_ptr<chaos::TiledMap::Map> tiled_map;
	};

	/** TiledMapLevelInstance : instance of a TiledMapLevel */
	class TiledMapLevelInstance : public GameLevelInstance
	{
	public:


	};

}; // namespace death
