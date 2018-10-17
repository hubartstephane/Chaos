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
public:

	/** constructor */
	LudumLevel(class LudumGame * in_game);

protected:

	/** pointer on game */
	class LudumGame * game = nullptr;

};

class LudumLevelInstance : public death::GameLevelInstance
{
public:

	/** constructor */
	LudumLevelInstance(class LudumGame * in_game);

protected:

	/** pointer on game */
	class LudumGame * game = nullptr;
};

// =================================================
// Levels
// =================================================

class LudumGameplayLevel : public LudumLevel
{
	friend class LudumGame;
	friend class LudumGameplayLevelInstance;

public:

	/** constructor */
	LudumGameplayLevel(class LudumGame * in_game);

protected:

	/** override */
	virtual death::GameLevelInstance * DoCreateLevelInstance() override;

protected:

	/** the tiled map corresponding to this level */
	boost::intrusive_ptr<chaos::TiledMap::Map> tiled_map;

};

// =================================================
// LevelInstance
// =================================================

class LudumGameplayLevelInstance : public LudumLevelInstance
{
public:

	/** constructor */
	LudumGameplayLevelInstance(class LudumGame * in_game);

protected:

	/** get the typed level */
	LudumGameplayLevel * GetLudumLevel();
	LudumGameplayLevel const * GetLudumLevel() const;

	/** overrides */
	virtual void OnLevelEnded() override;
	virtual void OnLevelStarted() override;

	/** get the particle layer from the level_layer */
	chaos::ParticleLayer * LevelLayerToParticleLayer(chaos::TiledMap::TileLayer const * level_layer) const;

protected:

	std::vector<boost::intrusive_ptr<chaos::ParticleAllocation>> allocations;
};
