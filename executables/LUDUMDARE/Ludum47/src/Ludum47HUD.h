#pragma once

#include <chaos/Chaos.h>

namespace chaos
{
	namespace GameHUDKeys
	{
		CHAOS_DECLARE_TAG(RACE_POSITION_ID);
		CHAOS_DECLARE_TAG(RACE_LAPS_ID);		
	};
};







// ====================================================================
// GameHUDRacePositionComponent
// ====================================================================

class GameHUDRacePositionComponent : public chaos::GameHUDCacheValueComponent<glm::ivec2>
{
public:


	/** constructor */
	GameHUDRacePositionComponent(chaos::TagType in_layer_id = chaos::GameHUDKeys::TEXT_LAYER_ID);
	/** constructor */
	GameHUDRacePositionComponent(chaos::ParticleTextGenerator::GeneratorParams const& in_params, chaos::TagType in_layer_id = chaos::GameHUDKeys::TEXT_LAYER_ID) :
		chaos::GameHUDCacheValueComponent<glm::ivec2>("Pos %d/%d", glm::ivec2(-1, -1), in_params, in_layer_id) {}

	virtual std::string FormatText() const override;

protected:

	/** override */
	virtual bool UpdateCachedValue(bool& destroy_allocation) override;
	/** override */
	virtual bool DoTick(float delta_time) override;


	float current_dt = 0.0f;

	float blink_timer = 0.0f;

	bool blink_value = false;
};


// ====================================================================
// GameHUDRaceLapsComponent
// ====================================================================

class GameHUDRaceLapsComponent : public chaos::GameHUDCacheValueComponent<glm::ivec2>
{
public:


	/** constructor */
	GameHUDRaceLapsComponent(chaos::TagType in_layer_id = chaos::GameHUDKeys::TEXT_LAYER_ID);
	/** constructor */
	GameHUDRaceLapsComponent(chaos::ParticleTextGenerator::GeneratorParams const& in_params, chaos::TagType in_layer_id = chaos::GameHUDKeys::TEXT_LAYER_ID) :
		chaos::GameHUDCacheValueComponent<glm::ivec2>("Pos %d/%d", glm::ivec2(-1, -1), in_params, in_layer_id) {}

	virtual std::string FormatText() const override;

protected:

	/** override */
	virtual bool UpdateCachedValue(bool& destroy_allocation) override;



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
	/** override */
	virtual int CreateHUDLayers() override;
};



