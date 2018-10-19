#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>
#include <chaos/TiledMap.h>
#include <chaos/TiledMapTools.h>

#include <death/GameLevel.h>

// =================================================
// Levels
// =================================================

class LudumLevel : public death::GameLevel
{
	friend class LudumGame;

public:


protected:

	/** override */
	virtual death::GameLevelInstance * DoCreateLevelInstance(death::Game * in_game) override;

protected:

	/** the tiled map corresponding to this level */
	boost::intrusive_ptr<chaos::TiledMap::Map> tiled_map;

};

// =================================================
// LevelInstance
// =================================================

class LudumLevelInstance : public death::GameLevelInstance
{
public:

};