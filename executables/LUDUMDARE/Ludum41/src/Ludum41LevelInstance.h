#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>

#include <death/Game.h>
#include <death/GameLevel.h>
#include <death/GameLevelInstance.h>
#include <death/GameFramework.h>

// =================================================
// LevelInstance
// =================================================

class LudumLevelInstance : public death::GameLevelInstance
{
public:

	DEATH_GAMEFRAMEWORK_DECLARE_LEVELINSTANCE(Ludum);

	/** override */
	virtual bool IsLevelCompleted() const override;
	/** override */
	virtual bool CanCompleteLevel() const override;
	/** override */
	virtual bool DoTick(double delta_time) override;
};