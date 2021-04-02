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
	GameHUDWakenParticleComponent();
	/** constructor */
	GameHUDWakenParticleComponent(chaos::ParticleTextGenerator::GeneratorParams const & in_params) :
		chaos::GameHUDCacheValueComponent<int>("Particles: %d", -1, in_params) {}

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

	CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);

	CHAOS_DECLARE_OBJECT_CLASS2(LudumPlayingHUD, chaos::PlayingHUD);

protected:

	/** override */
	virtual bool FillHUDContent() override;
};
