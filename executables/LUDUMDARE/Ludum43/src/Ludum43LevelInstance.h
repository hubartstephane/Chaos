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

	/** constructor */
	LudumLevelInstance(class LudumGame * in_game);

	DEATH_GAMEFRAMEWORK_DECLARE_LEVELINSTANCE(Ludum);

	/** get the current time out */
	float GetLevelTimeout() const { return level_timeout; }

protected:

	virtual bool CheckGameOverCondition() override;

	virtual bool IsLevelCompleted() const override;

	virtual bool DoTick(double delta_time) override;
	
	virtual void OnLevelStarted() override;

protected:

	/** pointer on game */
	class LudumGame * game = nullptr;

	/** the time to run the level */
	float level_timeout = 0.0f;
	/** the default level requirement */
	int level_particle_requirement = 0;
};
