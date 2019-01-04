#pragma once

#include <death/Game.h>
#include <death/GameHUD.h>

namespace death
{

	namespace GameHUDKeys
	{
		CHAOS_DECLARE_TAG(LEVEL_TIMEOUT_ID);
		CHAOS_DECLARE_TAG(WAKENUP_PARTICLE_COUNT_ID);
		CHAOS_DECLARE_TAG(SAVED_PARTICLE_COUNT_ID);
		CHAOS_DECLARE_TAG(LIFE_LAYER_ID);
		CHAOS_DECLARE_TAG(FPS_ID);
	};

};

class LudumPlayingHUD : public death::GameHUD
{

public:

	/** constructor */
	LudumPlayingHUD(death::Game * in_game) :
		death::GameHUD(in_game) {}

protected:

	/** override */
	virtual bool DoTick(double delta_time) override;

	virtual bool CreateHUDLayers() override;

	void UpdateLevelTimer(class LudumGame const * ludum_game);

	void UpdateWakenUpParticleCount(class LudumGame const * ludum_game);

	void UpdateSavedParticleCount(class LudumGame const * ludum_game);

	void UpdateFrameRate(class LudumGame const * ludum_game);

	void UpdateLifeBar(class LudumGame const * ludum_game);
	
	chaos::ParticleAllocation * CreateLevelTimeAllocation(float level_timeout, chaos::box2 const & view);

protected:

	/** caching particles count  */
	int cached_waken_up_particle_count = -1;
	/** caching saved particles count  */
	int cached_saved_particle_count = -1;
	/** caching the level time */
	float cached_level_timeout = -1.0f;
	/** caching the life value */
	float cached_life_value = -1.0f;
	/** caching the framerate value */
	float cached_framerate = -1.0f;

};
