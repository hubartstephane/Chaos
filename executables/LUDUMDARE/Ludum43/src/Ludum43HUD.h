#pragma once

#include <death/Game.h>
#include <death/GameHUD.h>
#include <death/GameFramework.h>

namespace death
{
	namespace GameHUDKeys
	{
		CHAOS_DECLARE_TAG(LEVEL_TIMEOUT_ID);
		CHAOS_DECLARE_TAG(WAKENUP_PARTICLE_COUNT_ID);
	};
};


// ====================================================================
// GameHUDWakenParticleComponent
// ====================================================================

class GameHUDWakenParticleComponent : public death::GameHUDCacheValueComponent<int>
{
public:

	GameHUDWakenParticleComponent() : death::GameHUDCacheValueComponent<int>("Particles : %d", -1) {}

protected:

	/** override */
	virtual bool UpdateCachedValue(bool & destroy_allocation) override;
	/** override */
	virtual void TweakTextGeneratorParams(chaos::ParticleTextGenerator::GeneratorParams & params, chaos::box2 const & view_box) override;
};



class LudumPlayingHUD : public death::PlayingHUD
{
public:

	DEATH_GAMEFRAMEWORK_DECLARE_HUD(Ludum);

	/** constructor */
	LudumPlayingHUD(death::Game * in_game) :
		death::PlayingHUD(in_game) {}

protected:

	/** override */
	virtual bool DoTick(double delta_time) override;
	/** override */
	virtual bool FillHUDContent() override;

	virtual bool CreateHUDLayers() override;

	void UpdateLevelTimer();

	void UpdateLifeBar();
	
	chaos::ParticleAllocation * CreateLevelTimeAllocation(float level_timeout, chaos::box2 const & view);

protected:

	/** caching the level time */
	float cached_level_timeout = -1.0f;
	/** caching the life value */
	float cached_value = -1.0f;
};
