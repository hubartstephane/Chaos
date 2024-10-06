#pragma once
#include "Ludum49PCH.h"
#include "Ludum49.h"
#include "Ludum49Particles.h"
#include "Ludum49Level.h"

// =========================================================
// LudumPlayer
// =========================================================

class LudumPlayer : public Player
{

public:


    CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);
    friend class LudumCameraComponent;

	CHAOS_DECLARE_OBJECT_CLASS(LudumPlayer, Player);


    class ParticlePlayer* GetPlayerParticle();

    class ParticlePlayer const* GetPlayerParticle() const;


protected:

    virtual bool DoTick(float delta_time) override;

    /** override */
    virtual bool Initialize(GameInstance* in_game_instance);

	/** override */
	virtual void OnLifeLost() override;
    /** override */
    virtual void OnLevelChanged(Level* new_level, Level* old_level, LevelInstance* new_level_instance) override;
    /** override */
    virtual void HandleInputs(float delta_time, GamepadState const* gpd) override;
    /** override */
    virtual bool OnReadConfigurableProperties(JSONReadConfiguration config, ReadConfigurablePropertiesContext context) override;


public:

    float gravity = 10.0f;

    float acceleration = 10.0f;
    float slow_down = 10.0f;
    float slow_down_factor = 0.05f;
    float stop_velocity = 0.0f;
    float max_velocity = 1000.0f;;
    float touching_ground_lapse_time = 0.05f;






    shared_ptr<GPUMesh> mesh;
};

