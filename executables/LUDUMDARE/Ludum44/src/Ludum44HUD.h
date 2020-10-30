#pragma once

#include <chaos/Chaos.h>

#include "Ludum44PowerUp.h"

namespace chaos
{
	namespace GameHUDKeys
	{
		CHAOS_DECLARE_TAG(POWER_UP_ID);
	};
};

// ====================================================================
// GameHUDLifeCountComponent
// ====================================================================

class GameHUDLifeCountComponent : public chaos::GameHUDCacheValueComponent<int>
{
public:

	/** constructor */
	GameHUDLifeCountComponent(chaos::TagType in_layer_id = chaos::GameHUDKeys::TEXT_LAYER_ID);
	/** constructor */
	GameHUDLifeCountComponent(chaos::ParticleTextGenerator::GeneratorParams const & in_params, chaos::TagType in_layer_id = chaos::GameHUDKeys::TEXT_LAYER_ID) :
		GameHUDCacheValueComponent<int>("Life: %d", -1, in_params, in_layer_id) {}

protected:

	/** override */
	virtual bool UpdateCachedValue(bool & destroy_allocation) override;
};

// ====================================================================
// GameHUDHealthBarComponent
// ====================================================================

class GameHUDHealthBarComponent : public chaos::GameHUDSingleAllocationComponent
{
protected:

	/** override */
	virtual bool DoTick(float delta_time) override;
};

// ====================================================================
// GameHUDPowerUpComponent
// ====================================================================

class GameHUDPowerUpComponent : public chaos::GameHUDSingleAllocationComponent
{
	friend class chaos::GameHUD;

protected:

	virtual bool DoTick(float delta_time) override;

protected:

	chaos::weak_ptr<LudumPowerUp> cached_power_up;
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
	/** override */
	virtual int CreateHUDLayers() override;
};
