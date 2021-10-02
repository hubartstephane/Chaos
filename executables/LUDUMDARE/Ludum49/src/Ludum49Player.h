#pragma once

#include <chaos/Chaos.h>

#include "Ludum49Particles.h"
#include "Ludum49Level.h"

// =========================================================
// LudumPlayer
// =========================================================

class LudumPlayer : public chaos::Player
{

public:


    CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);
    friend class LudumCameraComponent;

	CHAOS_DECLARE_OBJECT_CLASS2(LudumPlayer, chaos::Player);


    class ParticlePlayer* GetPlayerParticle();

    class ParticlePlayer const* GetPlayerParticle() const;


protected:

    virtual bool DoTick(float delta_time) override;

    /** override */
    virtual bool Initialize(chaos::GameInstance* in_game_instance);

	/** override */
	virtual void OnLifeLost() override;
    /** override */
    virtual void OnLevelChanged(chaos::Level* new_level, chaos::Level* old_level, chaos::LevelInstance* new_level_instance) override;
    /** override */
    virtual void HandleInputs(float delta_time, chaos::GamepadState const* gpd) override;
    /** override */
    virtual bool InitializeGameValues(nlohmann::json const& config, bool hot_reload) override;

public:





};

