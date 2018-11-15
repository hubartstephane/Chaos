#pragma once

#include <death/Game.h>
#include <death/GameHUD.h>

namespace death
{
	namespace GameHUDKeys
	{
		CHAOS_DECLARE_TAG(COMBO_ID);

		CHAOS_DECLARE_TAG(BACKGROUND_GAMEOBJECT_LAYER_ID);
		CHAOS_DECLARE_TAG(GAMEOBJECT_LAYER_ID);
		CHAOS_DECLARE_TAG(BRICK_LAYER_ID);
		CHAOS_DECLARE_TAG(LIFE_LAYER_ID);
		CHAOS_DECLARE_TAG(BALL_LAYER_ID);
		CHAOS_DECLARE_TAG(CHALLENGE_LAYER_ID);
	};
};

class LudumPlayingHUD : public death::PlayingHUD
{

public:

	/** constructor */
	LudumPlayingHUD(death::Game * in_game) :
		death::PlayingHUD(in_game) {}

	/** returns the current heart warning value */
	float GetHeartWarningValue() const { return heart_warning; }

protected:

	/** override */
	virtual bool DoTick(double delta_time) override;
	/** override */
	virtual bool InitializeHUD() override;

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
};
