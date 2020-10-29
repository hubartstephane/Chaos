#pragma once

#include <chaos/Chaos.h>

#include "Ludum45Particles.h"

// =========================================================
// LudumGameInstance
// =========================================================

class LudumGameInstance : public chaos::GameInstance
{

public:

	DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(Ludum);

	CHAOS_OBJECT_DECLARE_CLASS2(LudumGameInstance, chaos::GameInstance);

	LudumGameInstance();

	void FireExplosion(chaos::box2 const & ref_box);

protected:

	virtual void OnLevelChanged(chaos::Level * new_level, chaos::Level * old_level, chaos::LevelInstance * new_level_instance) override;
};
