#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>
#include <chaos/TiledMap.h>
#include <chaos/TiledMapTools.h>

#include <death/GameLevel.h>
#include <death/TiledMapLevel.h>
#include <death/Game.h>
#include <death/GameFramework.h>

#include "Ludum43Game.h"

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
	virtual bool IsLevelCompleted(bool & loop_levels) const override;
	/** override */
	virtual bool CanCompleteLevel() const override;
	/** override */
	virtual void CreateBackgroundImage() override;
	/** override */
	virtual void CreateCameras() override;
	/** override */
	virtual bool Initialize(death::Game * in_game, death::GameLevel * in_level) override;

protected:

	/** pointer on game */
	class LudumGame * game = nullptr;
};
