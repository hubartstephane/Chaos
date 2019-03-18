#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/NamedObject.h>

namespace death
{
	// Create a gamespace for unique idenfiers
	namespace GameHUDKeys
	{
		// some allocation ID's
		CHAOS_DECLARE_TAG(LIFE_ID);        // number of life
		CHAOS_DECLARE_TAG(LIFE_VITAE_ID);  // a floating life bar
		CHAOS_DECLARE_TAG(TITLE_ID);
		CHAOS_DECLARE_TAG(INSTRUCTIONS_ID);
		CHAOS_DECLARE_TAG(SCORE_ID);
		CHAOS_DECLARE_TAG(BEST_SCORE_ID);
		CHAOS_DECLARE_TAG(BACKGROUND_ID);
		CHAOS_DECLARE_TAG(FPS_ID);
		CHAOS_DECLARE_TAG(LEVEL_TIMEOUT_ID);
		// some layer ID's
		CHAOS_DECLARE_TAG(GAME_LAYER_ID);
		CHAOS_DECLARE_TAG(PLAYER_LAYER_ID);
		CHAOS_DECLARE_TAG(HUD_LAYER_ID);
		CHAOS_DECLARE_TAG(TEXT_LAYER_ID);
		CHAOS_DECLARE_TAG(BACKGROUND_LAYER_ID);
		CHAOS_DECLARE_TAG(LIFE_LAYER_ID);
		
	};

}; // namespace death
