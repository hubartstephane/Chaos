#pragma once

#include <chaos/Chaos.h>

#include "Ludum45Game.h"

// =================================================
// LudumLevelInstance
// =================================================

class LudumLevelInstance : public chaos::TMLevelInstance
{
public:

	DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(Ludum);

	CHAOS_OBJECT_DECLARE_CLASS2(LudumLevelInstance, chaos::TMLevelInstance);

protected:

	/** override */
	virtual bool DoTick(float delta_time) override;
	/** override */
	virtual bool Initialize(chaos::Game * in_game, chaos::Level * in_level) override;
	/** override */
	virtual void OnPlayerLeaved(chaos::Player * player) override;
	/** override */
	virtual void CreateCameras() override;

protected:

	/** pointer on game */
	class LudumGame * game = nullptr;


	float camera_speed = 100.0f;

};
