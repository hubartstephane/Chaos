#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/Object.h>
#include <chaos/TiledMap.h>
#include <chaos/TiledMapTools.h>

#include <death/Level.h>
#include <death/TiledMapLevel.h>
#include <death/Game.h>
#include <death/GameFramework.h>

#include "Ludum43Game.h"

// =================================================
// LudumLevelInstance
// =================================================

class LudumLevelInstance : public death::TiledMapLevelInstance
{
public:

	DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(Ludum);

	CHAOS_OBJECT_DECLARE_CLASS2(LudumLevelInstance, death::TiledMapLevelInstance);

protected:

	/** override */
	virtual bool CheckLevelCompletion() const override;
	/** override */
	virtual bool CanCompleteLevel() const override;
	/** override */
	virtual void CreateCameras() override;
	/** override */
	virtual bool Initialize(death::Game * in_game, death::Level * in_level) override;
	/** override */
	virtual death::PlayerPawn* CreatePlayerPawn(death::Player* player, death::TiledMapPlayerStart* player_start, death::TiledMapLayerInstance* layer_instance) override;

protected:

	/** pointer on game */
	class LudumGame * game = nullptr;
};
