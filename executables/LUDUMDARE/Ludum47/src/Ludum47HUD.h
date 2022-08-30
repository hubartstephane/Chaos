#pragma once

#include "Ludum47PCH.h"

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

class GameHUDRacePositionComponent : public chaos::GameHUDCacheValueTextComponent<glm::ivec2>
{
public:


	/** constructor */
	GameHUDRacePositionComponent();
	/** constructor */
	GameHUDRacePositionComponent(chaos::ParticleTextGenerator::GeneratorParams const& in_params) :
		chaos::GameHUDCacheValueTextComponent<glm::ivec2>("Pos %d/%d", in_params) {}

protected:

	/** override */
	virtual bool QueryValue(glm::ivec2& result) const override;
	/** override */
	virtual void UpdateMesh() override;
	/** override */
	virtual bool DoTick(float delta_time) override;

protected:

	float blink_timer = 0.0f;
	bool blink_value = false;
};


// ====================================================================
// GameHUDRaceLapsComponent
// ====================================================================

class GameHUDRaceLapsComponent : public chaos::GameHUDCacheValueTextComponent<glm::ivec2>
{
public:


	/** constructor */
	GameHUDRaceLapsComponent();
	/** constructor */
	GameHUDRaceLapsComponent(chaos::ParticleTextGenerator::GeneratorParams const& in_params) :
		chaos::GameHUDCacheValueTextComponent<glm::ivec2>("Pos %d/%d", in_params) {}

protected:

	/** override */
	virtual bool QueryValue(glm::ivec2 & result) const override;
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



