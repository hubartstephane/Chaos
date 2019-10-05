#pragma once

#include <death/Game.h>
#include <death/GameHUD.h>
#include <death/GameFramework.h>

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

// ====================================================================
// LudumPlayingHUD
// ====================================================================

class GameHUDUpgradeComponent : public death::GameHUDCacheValueComponent<std::string>
{
	friend class GameHUD;

public:

	/** constructor */
	GameHUDUpgradeComponent(chaos::TagType in_layer_id = death::GameHUDKeys::TEXT_LAYER_ID);
	/** constructor */
	GameHUDUpgradeComponent(chaos::ParticleTextGenerator::GeneratorParams const & in_params, chaos::TagType in_layer_id = death::GameHUDKeys::TEXT_LAYER_ID) :
		death::GameHUDCacheValueComponent<std::string>("%s", std::string(), in_params, in_layer_id) {}

protected:

	/** override */
	virtual std::string FormatText() const override;
	/** override */
	virtual bool UpdateCachedValue(bool & destroy_allocation) override;
};	
