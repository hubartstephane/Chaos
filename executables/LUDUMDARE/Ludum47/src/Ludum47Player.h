#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/BitmapAtlas.h>

#include <death/Game.h>
#include <death/GameInstance.h>
#include <death/Player.h>
#include <death/GameFramework.h>
#include "Ludum47Particles.h"
#include "Ludum47Level.h"

// =========================================================
// LudumPlayer
// =========================================================

class LudumPlayer : public death::Player
{

public:


    DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(Ludum);
    friend class LudumCameraComponent;

	CHAOS_OBJECT_DECLARE_CLASS2(LudumPlayer, death::Player);


    class ParticlePlayer* GetPlayerParticle();

    class ParticlePlayer const* GetPlayerParticle() const;


    void Honk();

    void SoundCollision();

    void SpawnSmokeParticles(size_t count);

protected:

    virtual bool DoTick(float delta_time) override;

    /** override */
    virtual bool Initialize(death::GameInstance* in_game_instance);

	/** override */
	virtual void OnLifeLost() override;
    /** override */
    virtual void OnLevelChanged(death::Level* new_level, death::Level* old_level, death::LevelInstance* new_level_instance) override;
    /** override */
    virtual void InternalHandleGamepadInputs(float delta_time, chaos::MyGLFW::GamepadData const* gpd) override;
    /** override */
    virtual void HandleKeyboardInputs(float delta_time) override;
    /** override */
    virtual bool InitializeGameValues(nlohmann::json const& config, boost::filesystem::path const& config_path, bool hot_reload) override;





public:

    chaos::shared_ptr<LudumRoad> road;

    chaos::shared_ptr<chaos::Sound> honk_sound;
    bool was_honk_pressed_keyboard = false;
    bool was_honk_pressed_gamepad = false;

    RacePosition race_position;

    float sound_collision_timer = 0.0f;

    float collision_health_lost = 1.0f;

    CarData car_data;
};

