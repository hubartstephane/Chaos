#pragma once

#include "chaos/Chaos.h"

#include "Ludum43Game.h"

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
	virtual bool CheckLevelCompletion() const override;
	/** override */
	virtual bool CanCompleteLevel() const override;
	/** override */
	virtual void CreateCameraComponents(chaos::Camera* camera, chaos::TMCameraTemplate* camera_template) override;
	/** override */
	virtual bool Initialize(chaos::Game * in_game, chaos::Level * in_level) override;
	/** override */
	virtual chaos::PlayerPawn* CreatePlayerPawnAtPlayerStart(chaos::Player* player, chaos::TMPlayerStart* player_start) override;

protected:

	/** pointer on game */
	class LudumGame * game = nullptr;
};
