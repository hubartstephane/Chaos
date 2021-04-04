#pragma once

#include <chaos/Chaos.h>

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

// ====================================================================
// LudumPlayingHUD
// ====================================================================

class GameHUDUpgradeComponent : public chaos::GameHUDCacheValueComponent<std::string>
{
	friend class GameHUD;

public:

	/** constructor */
	GameHUDUpgradeComponent();
	/** constructor */
	GameHUDUpgradeComponent(chaos::ParticleTextGenerator::GeneratorParams const & in_params) :
		chaos::GameHUDCacheValueComponent<std::string>("%s", std::string(), in_params) {}

protected:

	/** override */
	virtual std::string FormatText() const override;
	/** override */
	virtual bool UpdateCachedValue(bool & destroy_mesh) override;
};	









// ====================================================================
// GameHUDShroudLifeComponent
// ====================================================================


class GameHUDShroudLifeComponent : public chaos::GameHUDMeshComponent
{
	friend class GameHUD;

public:

	/** constructor */
	GameHUDShroudLifeComponent(char const* in_bitmap_name = ""):
		bitmap_name(in_bitmap_name) {}

	virtual bool InitializeFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path) override;

protected:

	virtual bool DoUpdateGPUResources(chaos::GPURenderer* renderer) override;

protected:

	std::string bitmap_name;

	int cached_value = -1;

	chaos::Hotpoint hotpoint = chaos::Hotpoint::BOTTOM_LEFT;
	glm::vec2 particle_size = glm::vec2(0.0f, 0.0f);
	glm::vec2 position = glm::vec2(0.0f, 0.0f);

};	


