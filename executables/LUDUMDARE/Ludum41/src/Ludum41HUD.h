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

protected:

	/** override */
	virtual bool DoTick(double delta_time) override;

public:

	void SetComboValue(death::Game * game, int new_combo);

protected:

	int cached_combo_value = -1;
};
