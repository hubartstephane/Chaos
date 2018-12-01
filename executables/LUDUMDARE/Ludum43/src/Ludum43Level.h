#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>
#include <chaos/TiledMap.h>
#include <chaos/TiledMapTools.h>

#include <death/GameLevel.h>
#include <death/TiledMapLevel.h>
#include <death/Game.h>

#include "Ludum43Game.h"


// =================================================
// Levels
// =================================================

class AtomGameObject : public death::TiledMap::GeometricObject
{
public:

	AtomGameObject(death::TiledMap::LayerInstance * in_layer_instance);


};

// =================================================
// Levels
// =================================================

class LudumLevel : public death::TiledMap::Level
{
	friend class LudumLevelInstance;

public:

	/** constructor */
	LudumLevel();

protected:

	/** override */
	virtual death::TiledMap::GeometricObject * DoCreateTypedObject(death::TiledMap::LayerInstance * in_layer_instance) override;

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

protected:

	/** pointer on game */
	class LudumGame * game = nullptr;
};
