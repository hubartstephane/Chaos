#pragma once

#include <death/Game.h>
#include <death/GameHUD.h>
#include <death/GameFramework.h>

#include "Ludum44PowerUp.h"

namespace death
{
	namespace GameHUDKeys
	{
		CHAOS_DECLARE_TAG(POWER_UP_ID);
	};
};

// ====================================================================
// GameHUDLifeCountComponent
// ====================================================================

class GameHUDLifeCountComponent : public death::GameHUDCacheValueComponent<int>
{
public:

	/** constructor */
	GameHUDLifeCountComponent(chaos::TagType in_layer_id = death::GameHUDKeys::TEXT_LAYER_ID);
	/** constructor */
	GameHUDLifeCountComponent(chaos::ParticleTextGenerator::GeneratorParams const & in_params, chaos::TagType in_layer_id = death::GameHUDKeys::TEXT_LAYER_ID) :
		GameHUDCacheValueComponent<int>("Life: %d", -1, in_params, in_layer_id) {}

protected:

	/** override */
	virtual bool UpdateCachedValue(bool & destroy_allocation) override;
};

// ====================================================================
// GameHUDHealthBarComponent
// ====================================================================

class GameHUDHealthBarComponent : public death::GameHUDSingleAllocationComponent
{
protected:

	/** override */
	virtual bool DoTick(float delta_time) override;
};

// ====================================================================
// GameHUDPowerUpComponent
// ====================================================================

class GameHUDPowerUpComponent : public death::GameHUDSingleAllocationComponent
{
	friend class death::GameHUD;

protected:

	virtual bool DoTick(float delta_time) override;

protected:

	chaos::weak_ptr<LudumPowerUp> cached_power_up;
};

// ====================================================================
// LudumPlayingHUD
// ====================================================================

class LudumPlayingHUD : public death::PlayingHUD
{
public:

	DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(Ludum);

	CHAOS_OBJECT_DECLARE_CLASS2(LudumPlayingHUD, death::PlayingHUD);

protected:

	/** override */
	virtual bool FillHUDContent() override;
	/** override */
	virtual bool CreateHUDLayers() override;
};
