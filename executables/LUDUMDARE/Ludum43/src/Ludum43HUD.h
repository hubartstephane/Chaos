#pragma once

#include <death/Game.h>
#include <death/GameHUD.h>

namespace death
{

	namespace GameHUDKeys
	{
		CHAOS_DECLARE_TAG(WAKENUP_PARTICLE_COUNT_ID);
		CHAOS_DECLARE_TAG(SAVED_PARTICLE_COUNT_ID);

#if 0
		CHAOS_DECLARE_TAG(COMBO_ID);

		CHAOS_DECLARE_TAG(BACKGROUND_GAMEOBJECT_LAYER_ID);
		CHAOS_DECLARE_TAG(GAMEOBJECT_LAYER_ID);
		CHAOS_DECLARE_TAG(BRICK_LAYER_ID);
		CHAOS_DECLARE_TAG(LIFE_LAYER_ID);
		CHAOS_DECLARE_TAG(BALL_LAYER_ID);
		CHAOS_DECLARE_TAG(CHALLENGE_LAYER_ID);
#endif
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

	void UpdateWakenUpParticleCount(class LudumGame const * ludum_game);

	void UpdateSavedParticleCount(class LudumGame const * ludum_game);

#if 0
protected:

	/** override */
	virtual bool DoTick(double delta_time) override;
	/** override */
	virtual bool CreateHUDLayers() override;

	/** update the combo particles */
	void UpdateComboParticles(class LudumGame const * ludum_game);
	/** update the life particles */
	void UpdateLifeParticles(class LudumGame const * ludum_game);

protected:

	/** update the Heart Beat values */
	void TickHeartWarning(LudumGame * ludum_game, double delta_time);

protected:

	/** caching the combo value */
	int cached_combo_value = -1;
	/** caching the current life count */
	int cached_life_value = -1;

	/** the current heart warning timer value */
	float heart_warning = 0.0f;
	/** the heart warning period */
	float heart_beat_speed = 1.0f;
#endif

	
protected:

	/** caching particles count  */
	int cached_waken_up_particle_count = -1;
	/** caching saved particles count  */
	int cached_saved_particle_count = -1;

};
