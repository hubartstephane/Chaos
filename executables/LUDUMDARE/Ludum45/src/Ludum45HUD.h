#pragma once

#include <chaos/Chaos.h>

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

// ====================================================================
// LudumPlayingHUD
// ====================================================================

class GameHUDUpgradeComponent : public chaos::GameHUDCacheValueComponent<std::string>
{
	friend class GameHUD;

public:

	/** constructor */
	GameHUDUpgradeComponent(chaos::TagType in_layer_id = chaos::GameHUDKeys::TEXT_LAYER_ID);
	/** constructor */
	GameHUDUpgradeComponent(chaos::ParticleTextGenerator::GeneratorParams const & in_params, chaos::TagType in_layer_id = chaos::GameHUDKeys::TEXT_LAYER_ID) :
		chaos::GameHUDCacheValueComponent<std::string>("%s", std::string(), in_params, in_layer_id) {}

protected:

	/** override */
	virtual std::string FormatText() const override;
	/** override */
	virtual bool UpdateCachedValue(bool & destroy_allocation) override;
};	









// ====================================================================
// GameHUDShroudLifeComponent
// ====================================================================


class GameHUDShroudLifeComponent : public chaos::GameHUDSingleAllocationComponent
{
	friend class GameHUD;

public:

	/** constructor */
	void OnInsertedInHUD(char const * bitmap_name);

	virtual bool InitializeFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path) override;

protected:

	chaos::Hotpoint hotpoint = chaos::Hotpoint::BOTTOM_LEFT;
	glm::vec2 particle_size = glm::vec2(0.0f, 0.0f);
	glm::vec2 position = glm::vec2(0.0f, 0.0f);

};	


