#pragma once

#include <death/Game.h>
#include <death/GameHUD.h>
#include <death/GameFramework.h>





namespace death
{
	namespace GameHUDKeys
	{
		CHAOS_DECLARE_TAG(RACE_POSITION_ID);

	};
};







// ====================================================================
// GameHUDRacePositionComponent
// ====================================================================

class GameHUDRacePositionComponent : public death::GameHUDCacheValueComponent<glm::ivec2>
{
public:


	/** constructor */
	GameHUDRacePositionComponent(chaos::TagType in_layer_id = death::GameHUDKeys::TEXT_LAYER_ID);
	/** constructor */
	GameHUDRacePositionComponent(chaos::ParticleTextGenerator::GeneratorParams const& in_params, chaos::TagType in_layer_id = death::GameHUDKeys::TEXT_LAYER_ID) :
		death::GameHUDCacheValueComponent<glm::ivec2>("Position: %d/%d", glm::ivec2(-1, -1), in_params, in_layer_id) {}

	virtual std::string FormatText() const override;

protected:

	/** override */
	virtual bool UpdateCachedValue(bool& destroy_allocation) override;
};


// ====================================================================
// LudumPlayingHUD
// ====================================================================

class LudumPlayingHUD : public death::PlayingHUD
{
public:

	DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(Ludum);

	CHAOS_OBJECT_DECLARE_CLASS2(LudumPlayingHUD, death::PlayingHUD);

protected:

	/** override */
	virtual bool FillHUDContent() override;
	/** override */
	virtual int CreateHUDLayers() override;
};



