#pragma once

#include "Ludum50.h"
#include "Ludum50Game.h"
#include "Ludum50Level.h"

// =================================================
// LudumLevelInstance
// =================================================

class LudumLevelInstance : public TMLevelInstance
{
public:

	CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);

	CHAOS_DECLARE_OBJECT_CLASS(LudumLevelInstance, TMLevelInstance);

	LudumLevelInstance();


protected:

	/** override */
	virtual bool DoTick(float delta_time) override;
	/** override */
	virtual bool Initialize(Game * in_game, Level * in_level) override;
	/** override */
	virtual void CreateCameraComponents(Camera* camera, TMCameraTemplate* camera_template) override;

	/** override */
	virtual bool IsPlayerDead(Player* player) override;

	virtual bool CheckLevelCompletion() const override;

	virtual bool CanCompleteLevel() const override;

	virtual int DoDisplay(GPURenderer* renderer, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params) override;

	virtual uint64_t GetCollisionFlagByName(char const* name) const override;





public:

	/** pointer on game */
	class LudumGame * game = nullptr;


	mutable float completion_timer = -1.0f; // shu50 : due to Checkcomplete constness !!
	float completion_delay = 5.0f;

};
