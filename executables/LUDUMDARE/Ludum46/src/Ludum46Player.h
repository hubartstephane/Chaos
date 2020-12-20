#pragma once

#include <chaos/Chaos.h>

#include "Ludum46Particles.h"

// =========================================================
// LudumPlayer
// =========================================================

class LudumPlayer : public chaos::Player
{

public:


    CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);

	CHAOS_DECLARE_OBJECT_CLASS2(LudumPlayer, chaos::Player);

	void AddBurnedSouls(int count) { burned_souls += count; }

protected:

    /** override */
    virtual bool Initialize(chaos::GameInstance* in_game_instance);

	/** override */
	virtual void OnLifeLost() override;
    /** override */
    virtual void OnLevelChanged(chaos::Level* new_level, chaos::Level* old_level, chaos::LevelInstance* new_level_instance) override;

protected:

	int burned_souls = 0;
};

