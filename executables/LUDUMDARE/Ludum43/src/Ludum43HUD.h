#pragma once

#include "Ludum43PCH.h"

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

class GameHUDWakenParticleComponent : public chaos::GameHUDCacheValueTextComponent<int>
{
public:


	/** constructor */
	GameHUDWakenParticleComponent();
	/** constructor */
	GameHUDWakenParticleComponent(chaos::ParticleTextGenerator::GeneratorParams const & in_params) :
		chaos::GameHUDCacheValueTextComponent<int>("Particles: %d", in_params) {}

protected:

	/** override */
	virtual bool QueryValue(int & result) const override;
};

// ====================================================================
// GameHUDHealthBarComponent
// ====================================================================

class GameHUDHealthInfo
{
public:

	friend bool operator == (GameHUDHealthInfo const& src1, GameHUDHealthInfo const& src2)
	{
		return (src1.health == src2.health) && (src1.max_health == src2.max_health);
	}

public:

	float health = 1.0f;
	float max_health = 1.0f;
};



class GameHUDHealthBarComponent : public chaos::GameHUDCacheValueMeshComponent<GameHUDHealthInfo>
{
protected:

	/** override */
	virtual bool QueryValue(GameHUDHealthInfo& result) const override;
	/** override */
	virtual void UpdateMesh() override;
};

// ====================================================================
// LudumPlayingHUD
// ====================================================================

class LudumPlayingHUD : public chaos::PlayingHUD
{
public:

	CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);

	CHAOS_DECLARE_OBJECT_CLASS(LudumPlayingHUD, chaos::PlayingHUD);

protected:

	/** override */
	virtual bool FillHUDContent() override;
};
