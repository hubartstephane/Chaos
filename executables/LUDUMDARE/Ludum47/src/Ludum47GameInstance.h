#pragma once

#include "chaos/Chaos.h"

#include "Ludum47Particles.h"

// =========================================================
// LudumGameInstance
// =========================================================

class LudumGameInstance : public chaos::GameInstance
{

public:

	CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);

	CHAOS_DECLARE_OBJECT_CLASS(LudumGameInstance, chaos::GameInstance);

	LudumGameInstance();

protected:

	virtual void OnLevelChanged(chaos::Level * new_level, chaos::Level * old_level, chaos::LevelInstance * new_level_instance) override;
};
