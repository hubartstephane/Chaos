#pragma once

#include "Ludum46PCH.h"
#include "Ludum46Game.h"

// =================================================
// LudumLevelInstance
// =================================================

class LudumLevelInstance : public chaos::TMLevelInstance
{
public:

	CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);

	CHAOS_DECLARE_OBJECT_CLASS(LudumLevelInstance, chaos::TMLevelInstance);

	LudumLevelInstance();

	void SpawnBloodParticles(chaos::box2 const& box, int particles_count);

	void SpawnBurnedSoulParticles(chaos::box2 const& box, int particles_count);

protected:

	/** override */
	virtual bool DoTick(float delta_time) override;
	/** override */
	virtual bool Initialize(chaos::Game * in_game, chaos::Level * in_level) override;
	/** override */
	virtual void CreateCameraComponents(chaos::Camera* camera, chaos::TMCameraTemplate* camera_template) override;


	virtual int DoDisplay(chaos::GPURenderContext* render_context, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::GPURenderParams const& render_params) override;

	/** override */
	virtual bool IsPlayerDead(chaos::Player* player) override;

	virtual bool CheckLevelCompletion() const override;

	virtual bool CanCompleteLevel() const override;

	int GetCurrentSoulCount() const;

	int GetPotentialSoulCount() const;






protected:

	/** pointer on game */
	class LudumGame * game = nullptr;


	mutable float completion_timer = -1.0f; //shu46 : due to Checkcomplete constness !!
	float completion_delay = 3.0f;

	float flame_health = 100.0f;
	float flame_initial_health = 100.0f;
	float flame_health_lost_per_second = 1.0f;
};
