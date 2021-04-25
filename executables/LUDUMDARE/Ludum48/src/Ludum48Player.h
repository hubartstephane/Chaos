#pragma once

#include <chaos/Chaos.h>

#include "Ludum48Particles.h"

// =========================================================
// LudumPlayer
// =========================================================

class LudumPlayer : public chaos::Player
{

public:


    CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);

	CHAOS_DECLARE_OBJECT_CLASS2(LudumPlayer, chaos::Player);

protected:

    /** override */
    virtual bool Initialize(chaos::GameInstance* in_game_instance);

	/** override */
	virtual void OnLifeLost() override;
    /** override */
    virtual void OnLevelChanged(chaos::Level* new_level, chaos::Level* old_level, chaos::LevelInstance* new_level_instance) override;

	virtual bool IsDead() const override;

    virtual bool DoTick(float delta_time) override;

public:

    float death_timer = -1.0f;

    float max_death_timer = 3.0f;
};

