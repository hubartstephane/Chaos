#pragma once
#include "Ludum50.h"
#include "Ludum50Particles.h"
#include "Ludum50Level.h"

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
    virtual bool InitializeGameValues(nlohmann::json const& config, bool hot_reload) override;


public:

    




    shared_ptr<GPUMesh> mesh;
};

