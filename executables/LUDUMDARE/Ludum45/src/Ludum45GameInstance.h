#pragma once

#include "Ludum45Particles.h"

#include <chaos/Chaos.h>

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

	CHAOS_OBJECT_DECLARE_CLASS2(LudumGameInstance, death::GameInstance);

	LudumGameInstance();

	void FireExplosion(chaos::box2 const & ref_box);

protected:

	virtual void OnLevelChanged(death::Level * new_level, death::Level * old_level, death::LevelInstance * new_level_instance) override;
};
