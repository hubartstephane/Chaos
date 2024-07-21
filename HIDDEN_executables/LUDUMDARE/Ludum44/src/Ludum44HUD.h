#pragma once

#include "Ludum44PCH.h"
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

class GameHUDLifeCountComponent : public chaos::GameHUDCacheValueTextComponent<int>
{
public:

	/** constructor */
	GameHUDLifeCountComponent();
	/** constructor */
	GameHUDLifeCountComponent(chaos::ParticleTextGenerator::GeneratorParams const & in_params) :
		GameHUDCacheValueTextComponent<int>("Life: %d", in_params) {}

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
// GameHUDPowerUpComponent
// ====================================================================

class GameHUDPowerUpComponent : public chaos::GameHUDCacheValueMeshComponent<chaos::weak_ptr<LudumPowerUp>>
{
	friend class chaos::GameHUD;

protected:

	virtual void UpdateMesh() override;

	virtual bool QueryValue(chaos::weak_ptr<LudumPowerUp> & result) const override;
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
