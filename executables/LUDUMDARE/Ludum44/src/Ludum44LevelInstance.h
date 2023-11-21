#pragma once

#include "Ludum44PCH.h"
#include "Ludum44Game.h"

// =================================================
// LudumLevelInstance
// =================================================

class LudumLevelInstance : public chaos::TMLevelInstance
{
public:

	CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);

	CHAOS_DECLARE_OBJECT_CLASS(LudumLevelInstance, chaos::TMLevelInstance);

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
	virtual void CreateCameraComponents(chaos::Camera* camera, chaos::TMCameraTemplate* camera_template) override;

	/** override */
	bool SerializeFromJSON(nlohmann::json const * json) override;
	/** override */
	bool SerializeIntoJSON(nlohmann::json * json) const override;

protected:

	/** pointer on game */
	class LudumGame * game = nullptr;

	/** the horizontal camera speed */
	float camera_speed = 100.0f;
	/** an additional factor that can be applyed on demand */
	float scroll_factor = 1.0f;
};
