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

	void UpdateWakenUpParticleCount();

	void UpdateLifeBar();
	
	chaos::ParticleAllocation * CreateLevelTimeAllocation(float level_timeout, chaos::box2 const & view);

protected:

	/** caching particles count  */
	int cached_waken_up_particle_count = -1;
	/** caching saved particles count  */
	int cached_saved_particle_count = -1;
	/** caching the level time */
	float cached_level_timeout = -1.0f;
	/** caching the life value */
	float cached_value = -1.0f;
};
