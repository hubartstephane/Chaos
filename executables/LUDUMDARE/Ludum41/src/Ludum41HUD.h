#pragma once

#include <death/Game.h>
#include <death/GameHUD.h>

namespace death
{
	namespace GameHUDKeys
	{
		CHAOS_DECLARE_TAG(COMBO_ID);

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

protected:

	/** caching the combo value */
	int cached_combo_value = -1;
};
