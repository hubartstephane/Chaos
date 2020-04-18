#pragma once

#include "Ludum46GameCheckpoint.h"
#include "Ludum46Particles.h"

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

	DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(Ludum);

	/** constructor */
	LudumGameInstance(death::Game * in_game);

protected:

	virtual death::Player * DoCreatePlayer() override;

	virtual void OnLevelChanged(death::Level * new_level, death::Level * old_level, death::LevelInstance * new_level_instance) override;
};
