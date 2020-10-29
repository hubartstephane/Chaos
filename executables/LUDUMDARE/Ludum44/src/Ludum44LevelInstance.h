#pragma once

#include <chaos/Chaos.h>

#include "Ludum44Game.h"

// =================================================
// LudumLevelInstance
// =================================================

class LudumLevelInstance : public chaos::TMLevelInstance
{
public:

	DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(Ludum);

	CHAOS_OBJECT_DECLARE_CLASS2(LudumLevelInstance, chaos::TMLevelInstance);

	/** Set current scroll factor */
	void SetScrollFactor(float in_scroll_factor);
	/** Get current scroll factor */
	float GetScrollFactor() const;

protected:

	/** override */
	virtual bool DoTick(float delta_time) override;
	/** override */
	virtual void OnLevelStarted() override;
	/** override */
	virtual bool Initialize(chaos::Game * in_game, chaos::Level * in_level) override;
	/** override */
	virtual void CreateCameras() override;

	/** override */
	bool SerializeFromJSON(nlohmann::json const& json) override;
	/** override */
	bool SerializeIntoJSON(nlohmann::json & json) const override;

protected:

	/** pointer on game */
	class LudumGame * game = nullptr;

	/** the horizontal camera speed */
	float camera_speed = 100.0f;
	/** an additional factor that can be applyed on demand */
	float scroll_factor = 1.0f;
};
