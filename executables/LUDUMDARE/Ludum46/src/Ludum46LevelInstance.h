#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>
#include <chaos/TiledMap.h>
#include <chaos/TiledMapTools.h>

#include <death/Level.h>
#include <death/TiledMapLevel.h>
#include <death/Game.h>
#include <death/Player.h>
#include <death/GameFramework.h>

#include "Ludum46Game.h"

// =================================================
// LudumLevelInstance
// =================================================

class LudumLevelInstance : public death::TiledMapLevelInstance
{
public:

	DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(Ludum);

	/** constructor */
	LudumLevelInstance(class LudumGame * in_game);


protected:

	/** override */
	virtual bool DoTick(float delta_time) override;
	/** override */
	virtual void OnLevelStarted() override;
	/** override */
	virtual bool Initialize(death::Game * in_game, death::Level * in_level) override;
	/** override */
	virtual void OnPlayerEntered(death::Player * player) override;
	/** override */
	virtual void OnPlayerLeaved(death::Player * player) override;
	/** override */
	virtual void CreateCameras() override;
	/** override */
	virtual death::PlayerDisplacementComponent* CreatePlayerDisplacementComponent(death::Player* player) override;

	/** override */
	virtual death::LevelCheckpoint * DoCreateCheckpoint() const;
	/** override */
	virtual bool DoLoadFromCheckpoint(death::LevelCheckpoint const * checkpoint) override;
	/** override */
	virtual bool DoSaveIntoCheckpoint(death::LevelCheckpoint * checkpoint) const override;
	/** override */
	virtual bool IsPlayerDead(death::Player* player) override;

	int GetCurrentSoulCount() const;

	int GetPotentialSoulCount() const;




protected:

	/** pointer on game */
	class LudumGame * game = nullptr;

};
