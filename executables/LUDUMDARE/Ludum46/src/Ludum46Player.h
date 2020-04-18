#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/BitmapAtlas.h>

#include <death/Game.h>
#include <death/GameInstance.h>
#include <death/Player.h>
#include <death/GameFramework.h>
#include "Ludum46Particles.h"

// =========================================================
// LudumPlayer
// =========================================================

class LudumPlayer : public death::Player
{

public:


    DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(Ludum);

    /** constructor */
    LudumPlayer(death::GameInstance* in_game_instance);

protected:

    /** override */
    virtual bool Initialize(death::GameInstance* in_game_instance);
    /** override */
    virtual death::PlayerCheckpoint* DoCreateCheckpoint() const override;
    /** override */
    virtual bool DoLoadFromCheckpoint(death::PlayerCheckpoint const* checkpoint) override;
    /** override */
    virtual bool DoSaveIntoCheckpoint(death::PlayerCheckpoint* checkpoint) const override;

    /** override */
    virtual void OnLevelChanged(death::Level* new_level, death::Level* old_level, death::LevelInstance* new_level_instance) override;
    /** override */
    virtual void InternalHandleGamepadInputs(float delta_time, chaos::MyGLFW::GamepadData const* gpd) override;
    /** override */
    virtual void HandleKeyboardInputs(float delta_time) override;


};

