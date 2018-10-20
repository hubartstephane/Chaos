#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>
#include <chaos/TiledMap.h>
#include <chaos/TiledMapTools.h>

#include <death/GameLevel.h>

#include "LudumFailedGame.h"

// =================================================
// Levels
// =================================================

class LudumLevel : public death::GameLevel
{
	friend class LudumGame;

public:

	/** constructor */
	LudumLevel();

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

	/** constructor */
	LudumLevelInstance(class LudumGame * in_game);

protected:

	/** get the typed level */
	LudumLevel * GetTypedLevel();
	LudumLevel const * GetTypedLevel() const;

	/** overrides */
	virtual void OnLevelEnded() override;
	virtual void OnLevelStarted() override;

	/** get the particle layer from the level_layer */
	chaos::ParticleLayer * LevelLayerToParticleLayer(chaos::TiledMap::TileLayer const * level_layer) const;

protected:

	LudumGame * game = nullptr;

	std::vector<boost::intrusive_ptr<chaos::ParticleAllocation>> allocations;
};
