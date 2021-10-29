#pragma once

#include "Ludum48.h"
#include "Ludum48Particles.h"

// =========================================================
// LudumPlayer
// =========================================================

class LudumPlayer : public Player
{

public:


    CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);

	CHAOS_DECLARE_OBJECT_CLASS(LudumPlayer, Player);

protected:

    /** override */
    virtual bool Initialize(GameInstance* in_game_instance);

	/** override */
	virtual void OnLifeLost() override;
    /** override */
    virtual void OnLevelChanged(Level* new_level, Level* old_level, LevelInstance* new_level_instance) override;

	virtual bool IsDead() const override;

    virtual bool DoTick(float delta_time) override;

    virtual bool InitializeGameValues(nlohmann::json const& config, bool hot_reload) override;

public:

    float death_timer = -1.0f;

    float max_death_timer = 1.0f;

    float suicidal_timer = -1.0f;

    float max_suicidal_timer = 1.0f;

    bool cheater_farid = false;
};

