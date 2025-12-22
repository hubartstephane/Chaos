#pragma once

#include "Ludum47PCH.h"
#include "Ludum47Particles.h"
#include "Ludum47Level.h"

// =========================================================
// LudumPlayer
// =========================================================

class LudumPlayer : public chaos::Player
{

public:


    CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);
    friend class LudumCameraComponent;

	CHAOS_DECLARE_OBJECT_CLASS(LudumPlayer, chaos::Player);


    class ParticlePlayer* GetPlayerParticle();

    class ParticlePlayer const* GetPlayerParticle() const;


    void Honk();

    void SoundCollision();

    static void SpawnSmokeParticles(size_t count, LudumLevelInstance* li, chaos::obox2 const& ob);

protected:

    virtual bool DoTick(float delta_time) override;

    /** override */
    virtual bool Initialize(chaos::GameInstance* in_game_instance);

	/** override */
	virtual void OnLifeLost() override;
    /** override */
    virtual void OnLevelChanged(chaos::Level* new_level, chaos::Level* old_level, chaos::LevelInstance* new_level_instance) override;

    virtual bool EnumerateInputActions(chaos::InputActionEnumerator& in_action_enumerator, chaos::EnumerateInputActionContext in_context) override;

    /** override */
    virtual bool OnReadConfigurableProperties(chaos::JSONReadConfiguration config, chaos::ReadConfigurablePropertiesContext context) override;

public:

    chaos::shared_ptr<LudumRoad> road;

    chaos::shared_ptr<chaos::Sound> honk_sound;

    float velocity_collision_factor = 0.7f;

    RacePosition race_position;

    float sound_collision_timer = 0.0f;

    float collision_health_lost = 1.0f;

    CarData car_data;

    ParticleSpawnerDelay spawner_delay;

};

