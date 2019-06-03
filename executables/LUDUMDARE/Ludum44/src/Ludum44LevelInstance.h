#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>
#include <chaos/TiledMap.h>
#include <chaos/TiledMapTools.h>

#include <death/GameLevel.h>
#include <death/TiledMapLevel.h>
#include <death/Game.h>
#include <death/Player.h>
#include <death/GameFramework.h>

#include "Ludum44Game.h"

// =================================================
// LudumLevelInstance
// =================================================

class LudumLevelInstance : public death::TiledMap::LevelInstance
{
public:

	DEATH_GAMEFRAMEWORK_DECLARE_LEVELINSTANCE(Ludum);

	/** constructor */
	LudumLevelInstance(class LudumGame * in_game);

	/** Set current scroll factor */
	void SetScrollFactor(float in_scroll_factor){ scroll_factor = in_scroll_factor;}
	/** Get current scroll factor */
	float GetScrollFactor() const { return scroll_factor; }
	/** set the change level status */
	void SetLevelCompleted(bool in_value){ level_completed = in_value;}

protected:

	/** override */
	virtual bool IsLevelCompleted(bool & loop_levels) const override;
	/** override */
	virtual bool CanCompleteLevel() const override;
	/** override */
	virtual bool DoTick(double delta_time) override;
	/** override */
	virtual void OnLevelStarted() override;
	/** override */
	virtual bool Initialize(death::Game * in_game, death::GameLevel * in_level) override;
	/** override */
	virtual void OnPlayerEntered(death::Player * player) override;
	/** override */
	virtual void OnPlayerLeaved(death::Player * player) override;
	/** override */
	virtual void CreateCameras() override;

protected:

	/** pointer on game */
	class LudumGame * game = nullptr;

	/** the horizontal camera speed */
	float camera_speed = 100.0f;
	/** an additional factor that can be applyed on demand */
	float scroll_factor = 1.0f;
	/** whether the level is completed */
	bool level_completed = false;
};
