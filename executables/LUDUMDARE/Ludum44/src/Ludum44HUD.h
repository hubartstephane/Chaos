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
	GameHUDLifeCountComponent();
	/** constructor */
	GameHUDLifeCountComponent(chaos::ParticleTextGenerator::GeneratorParams const & in_params) :
		GameHUDCacheValueComponent<int>("Life: %d", -1, in_params) {}

protected:

	/** override */
	virtual bool UpdateCachedValue(bool & destroy_mesh) override;
};

// ====================================================================
// GameHUDHealthBarComponent
// ====================================================================

class GameHUDHealthBarComponent : public chaos::GameHUDMeshComponent
{
protected:

	/** override */
	virtual bool DoTick(float delta_time) override;
};

// ====================================================================
// GameHUDPowerUpComponent
// ====================================================================

class GameHUDPowerUpComponent : public chaos::GameHUDMeshComponent
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
};
