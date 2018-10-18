#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>
#include <chaos/TiledMap.h>
#include <chaos/TiledMapTools.h>

#include <death/GameLevel.h>
#include <death/TiledMapLevel.h>

#include "Ludum42Game.h"


// =================================================
// Levels
// =================================================

class LudumGameplayLevel : public death::TiledMap::Level
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

	/** pointer on game */
	class LudumGame * game = nullptr;

};

// =================================================
// LevelInstance
// =================================================

class LudumGameplayLevelInstance : public death::GameLevelInstance
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
	chaos::ParticleLayer * ObjectToParticleLayer(int object_type) const;

	/** create an allocation for the object type */
	chaos::ParticleAllocation * FindOrAllocationForObjectType(int object_type);

protected:

	/** pointer on game */
	class LudumGame * game = nullptr;

	std::map<int, boost::intrusive_ptr<chaos::ParticleAllocation>> allocations;
};
