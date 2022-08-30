#pragma once
#include "Ludum50PCH.h"
#include "Ludum50.h"
#include "Ludum50Particles.h"

// =========================================================
// LudumGameInstance
// =========================================================

class LudumGameInstance : public GameInstance
{

public:

	CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);

	CHAOS_DECLARE_OBJECT_CLASS(LudumGameInstance, GameInstance);

	LudumGameInstance();

protected:

	virtual void OnLevelChanged(Level * new_level, Level * old_level, LevelInstance * new_level_instance) override;
};
