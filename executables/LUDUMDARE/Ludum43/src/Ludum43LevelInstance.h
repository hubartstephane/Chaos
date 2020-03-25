#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>
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

	/** constructor */
	LudumLevelInstance(class LudumGame * in_game);

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
	virtual death::PlayerPawn* CreatePlayerPawn(death::Player* player, death::TiledMapPlayerStartObject* player_start, char const* bitmap_name, death::TiledMapLayerInstance* layer_instance, chaos::box2 const& player_bounding_box) override;

protected:

	/** pointer on game */
	class LudumGame * game = nullptr;
};
