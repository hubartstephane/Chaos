#pragma once

#include "Ludum45PCH.h"

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

// ====================================================================
// LudumPlayingHUD
// ====================================================================

class GameHUDUpgradeComponent : public chaos::GameHUDCacheValueTextComponent<std::string>
{
	friend class GameHUD;

public:

	/** constructor */
	GameHUDUpgradeComponent();
	/** constructor */
	GameHUDUpgradeComponent(chaos::ParticleTextGenerator::GeneratorParams const & in_params) :
		chaos::GameHUDCacheValueTextComponent<std::string>("%s", in_params) {}



protected:

	/** override */
	virtual bool QueryValue(std::string & result) const override;
	/** override */
	virtual void UpdateMesh() override;
};

// ====================================================================
// GameHUDShroudLifeComponent
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


class GameHUDShroudLifeComponent : public chaos::GameHUDCacheValueMeshComponent<GameHUDHealthInfo>
{
	friend class GameHUD;

public:

	/** constructor */
	GameHUDShroudLifeComponent(char const* in_bitmap_name = ""):
		bitmap_name(in_bitmap_name) {}

	virtual bool InitializeFromConfiguration(nlohmann::json const& config) override;

protected:

	virtual bool QueryValue(GameHUDHealthInfo& result) const override;

	virtual void UpdateMesh() override;

protected:

	std::string bitmap_name;

	chaos::Hotpoint hotpoint = chaos::Hotpoint::BOTTOM_LEFT;
	glm::vec2 particle_size = glm::vec2(0.0f, 0.0f);
	glm::vec2 position = glm::vec2(0.0f, 0.0f);
};


