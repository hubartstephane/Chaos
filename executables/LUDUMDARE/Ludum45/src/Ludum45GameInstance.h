#pragma once

#include "Ludum45GameCheckpoint.h"
#include "Ludum45Particles.h"

#include <chaos/StandardHeaders.h> 
#include <chaos/ParticleManager.h> 
#include <chaos/BitmapAtlas.h> 
#include <chaos/GeometryFramework.h> 

#include <death/Game.h>
#include <death/GameInstance.h>
#include <death/GameFramework.h>


// =========================================================
// LudumGameInstance
// =========================================================

class LudumGameInstance : public death::GameInstance
{

public:

	DEATH_GAMEFRAMEWORK_DECLARE_GAMEINSTANCE(Ludum);

	/** constructor */
	LudumGameInstance(death::Game * in_game);


	void FireExplosion(chaos::box2 const & ref_box);

protected:

	virtual death::Player * DoCreatePlayer() override;

	virtual bool DoCheckGameOverCondition() override;

	virtual void OnLevelChanged(death::GameLevel * new_level, death::GameLevel * old_level, death::GameLevelInstance * new_level_instance) override;

	virtual void OnPlayerEntered(death::Player * player) override;
	
	virtual bool DoSaveIntoCheckpoint(death::GameCheckpoint * checkpoint) const override;
	
	virtual bool DoLoadFromCheckpoint(death::GameCheckpoint const * checkpoint) override;
};
