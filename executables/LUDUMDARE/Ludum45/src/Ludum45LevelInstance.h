#pragma once

#include "chaos/Chaos.h"

#include "Ludum45Game.h"

// =================================================
// LudumLevelInstance
// =================================================

class LudumLevelInstance : public chaos::TMLevelInstance
{
public:

	CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);

	CHAOS_DECLARE_OBJECT_CLASS(LudumLevelInstance, chaos::TMLevelInstance);

protected:

	/** override */
	virtual bool DoTick(float delta_time) override;
	/** override */
	virtual bool Initialize(chaos::Game * in_game, chaos::Level * in_level) override;
	/** override */
	virtual void OnPlayerLeaved(chaos::Player * player) override;
	/** override */
	virtual void CreateCameraComponents(chaos::Camera* camera, chaos::TMCameraTemplate* camera_template) override;

protected:

	/** pointer on game */
	class LudumGame * game = nullptr;


	float camera_speed = 100.0f;

};
