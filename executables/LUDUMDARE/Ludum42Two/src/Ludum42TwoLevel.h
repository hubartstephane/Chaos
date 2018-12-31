#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>
#include <chaos/TiledMap.h>
#include <chaos/TiledMapTools.h>

#include <death/GameLevel.h>
#include <death/TiledMapLevel.h>

#include "Ludum42TwoGame.h"

// =================================================
// Levels
// =================================================

class LudumLevel : public death::TiledMap::Level
{
	friend class LudumGame;
	friend class LudumLevelInstance;

public:

	/** constructor */
	LudumLevel();

protected:

	/** override */
	virtual death::GameLevelInstance * DoCreateLevelInstance(death::Game * in_game) override;
};

// =================================================
// LevelInstance
// =================================================

class LudumLevelInstance : public death::TiledMap::LevelInstance
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

	std::vector<chaos::shared_ptr<chaos::ParticleAllocation>> allocations;
};
