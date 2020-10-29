#pragma once

#include <chaos/Chaos.h>

namespace chaos
{
	namespace GameHUDKeys
	{
		CHAOS_DECLARE_TAG(WAKENUP_PARTICLE_COUNT_ID);
	};
};


// ====================================================================
// GameHUDWakenParticleComponent
// ====================================================================

class GameHUDWakenParticleComponent : public chaos::GameHUDCacheValueComponent<int>
{
public:


	/** constructor */
	GameHUDWakenParticleComponent(chaos::TagType in_layer_id = chaos::GameHUDKeys::TEXT_LAYER_ID);
	/** constructor */
	GameHUDWakenParticleComponent(chaos::ParticleTextGenerator::GeneratorParams const & in_params, chaos::TagType in_layer_id = chaos::GameHUDKeys::TEXT_LAYER_ID) :
		chaos::GameHUDCacheValueComponent<int>("Particles: %d", -1, in_params, in_layer_id) {}

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

protected:

	/** the last life value for the player */
	float cached_value = -1.0f;
};

// ====================================================================
// LudumPlayingHUD
// ====================================================================

class LudumPlayingHUD : public chaos::PlayingHUD
{
public:

	DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(Ludum);

	CHAOS_OBJECT_DECLARE_CLASS2(LudumPlayingHUD, chaos::PlayingHUD);

protected:

	/** override */
	virtual bool FillHUDContent() override;
	/** override */
	virtual int CreateHUDLayers() override;
};
