#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>
#include <chaos/TiledMap.h>
#include <chaos/TiledMapTools.h>

#include <death/GameLevel.h>
#include <death/TiledMapLevel.h>
#include <death/Game.h>
#include <death/Player.h>
#include <death/GameFramework.h>

#include "LudumTestGame.h"

// =================================================
// LudumLevelInstance
// =================================================

class LudumLevelInstance : public death::TiledMap::LevelInstance
{
public:

	DEATH_GAMEFRAMEWORK_DECLARE_LEVELINSTANCE(Ludum);

	/** constructor */
	LudumLevelInstance(class LudumGame * in_game);


protected:

	/** override */
	virtual bool DoTick(float delta_time) override;
	/** override */
	virtual void OnLevelStarted() override;
	/** override */
	virtual bool Initialize(death::Game * in_game, death::GameLevel * in_level) override;
	/** override */
	virtual void OnPlayerEntered(death::Player * player) override;
	/** override */
	virtual void OnPlayerLeaved(death::Player * player) override;
	/** override */
	virtual void CreateCameras() override;

	/** override */
	virtual death::LevelCheckpoint * DoCreateCheckpoint() const;
	/** override */
	virtual bool DoLoadFromCheckpoint(death::LevelCheckpoint const * checkpoint) override;
	/** override */
	virtual bool DoSaveIntoCheckpoint(death::LevelCheckpoint * checkpoint) const override;


protected:

	/** pointer on game */
	class LudumGame * game = nullptr;

};
