#pragma once

#include <chaos/Chaos.h>

#include "Ludum43Game.h"

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
	virtual bool CheckLevelCompletion() const override;
	/** override */
	virtual bool CanCompleteLevel() const override;
	/** override */
	virtual void CreateCameras() override;
	/** override */
	virtual bool Initialize(chaos::Game * in_game, chaos::Level * in_level) override;
	/** override */
	virtual chaos::PlayerPawn* CreatePlayerPawnAtPlayerStart(chaos::Player* player, chaos::TMPlayerStart* player_start) override;

protected:

	/** pointer on game */
	class LudumGame * game = nullptr;
};
