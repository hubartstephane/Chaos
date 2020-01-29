#pragma once

#include <death/Game.h>
#include <death/GameHUD.h>
#include <death/GameFramework.h>

namespace death
{
	namespace GameHUDKeys
	{
		CHAOS_DECLARE_TAG(WAKENUP_PARTICLE_COUNT_ID);
	};
};


// ====================================================================
// GameHUDWakenParticleComponent
// ====================================================================

class GameHUDWakenParticleComponent : public death::GameHUDCacheValueComponent<int>
{
public:


	/** constructor */
	GameHUDWakenParticleComponent(chaos::TagType in_layer_id = death::GameHUDKeys::TEXT_LAYER_ID);
	/** constructor */
	GameHUDWakenParticleComponent(chaos::ParticleTextGenerator::GeneratorParams const & in_params, chaos::TagType in_layer_id = death::GameHUDKeys::TEXT_LAYER_ID) :
		death::GameHUDCacheValueComponent<int>("Particles: %d", -1, in_params, in_layer_id) {}

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
	virtual bool DoTick(double delta_time) override;

protected:

	/** the last life value for the player */
	float cached_value = -1.0f;
};

// ====================================================================
// LudumPlayingHUD
// ====================================================================

class LudumPlayingHUD : public death::PlayingHUD
{
public:

	DEATH_GAMEFRAMEWORK_DECLARE_HUD(Ludum);

	/** constructor */
	LudumPlayingHUD(death::Game * in_game) :
		death::PlayingHUD(in_game) {}

protected:

	/** override */
	virtual bool FillHUDContent() override;
	/** override */
	virtual bool CreateHUDLayers() override;
};
