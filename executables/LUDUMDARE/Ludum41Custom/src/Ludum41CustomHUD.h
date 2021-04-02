#pragma once

#include <chaos/Chaos.h>

namespace chaos
{
	namespace GameHUDKeys
	{
		CHAOS_DECLARE_TAG(COMBO_ID);

		CHAOS_DECLARE_TAG(BACKGROUND_GAMEOBJECT_LAYER_ID);
		CHAOS_DECLARE_TAG(GAMEOBJECT_LAYER_ID);
		CHAOS_DECLARE_TAG(BRICK_LAYER_ID);
		CHAOS_DECLARE_TAG(BALL_LAYER_ID);
		CHAOS_DECLARE_TAG(CHALLENGE_LAYER_ID);
	};
};

// ====================================================================
// GameHUDComboComponent
// ====================================================================

class GameHUDComboComponent : public chaos::GameHUDCacheValueComponent<int>
{
public:

	/** constructor */
	GameHUDComboComponent();
	/** constructor */
	GameHUDComboComponent(chaos::ParticleTextGenerator::GeneratorParams const & in_params) :
		chaos::GameHUDCacheValueComponent<int>("Combo: %d x", -1, in_params) {}

protected:

	/** override */
	virtual bool UpdateCachedValue(bool & destroy_mesh) override;
};

// ====================================================================
// LudumPlayingHUD
// ====================================================================


class LudumPlayingHUD : public chaos::PlayingHUD
{

public:

	CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);

	CHAOS_DECLARE_OBJECT_CLASS2(LudumPlayingHUD, chaos::PlayingHUD);

protected:

	/** override */
	virtual bool FillHUDContent() override;

protected:

	/** caching the current life count */
	int cached_value = -1;
};
