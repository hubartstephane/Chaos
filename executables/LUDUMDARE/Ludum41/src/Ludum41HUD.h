#pragma once

#include <death/Game.h>
#include <death/GameHUD.h>

namespace death
{
	namespace GameHUDKeys
	{
		CHAOS_DECLARE_TAG(COMBO_ID);
		CHAOS_DECLARE_TAG(LIFE_ID);

		CHAOS_DECLARE_TAG(BACKGROUND_LAYER_ID);
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

	/** caching the combo value */
	int cached_combo_value = -1;
	/** caching the current life count */
	int cached_life_value = -1;
};
