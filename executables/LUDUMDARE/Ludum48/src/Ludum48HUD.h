#pragma once

#include "Ludum48.h"

// ====================================================================
// LudumPlayingHUD
// ====================================================================

class LudumPlayingHUD : public PlayingHUD
{
public:

	CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);

	CHAOS_DECLARE_OBJECT_CLASS(LudumPlayingHUD, PlayingHUD);

protected:

	/** override */
	virtual bool FillHUDContent() override;
};




// ====================================================================
// LudumHUDDiamondComponent
// ====================================================================

class LudumHUDDiamondComponent : public GameHUDCacheValueTextComponent<std::pair<int, int> >
{
	friend class GameHUD;

public:

	/** constructor */
	LudumHUDDiamondComponent(char const* in_text = "%d/%d");
	/** constructor */
	LudumHUDDiamondComponent(ParticleTextGenerator::GeneratorParams const& in_params) :
		GameHUDCacheValueTextComponent<std::pair<int, int>>("%d/%d", in_params) {}


protected:

	/** override */
	virtual bool QueryValue(std::pair<int, int>& result) const override;
	/** override */
	virtual void UpdateMesh() override;
};



class LudumHUDLifeComponent : public GameHUDCacheValueTextComponent<int>
{
	friend class GameHUD;

public:

	/** constructor */
	LudumHUDLifeComponent(char const* in_text = "%d");
	/** constructor */
	LudumHUDLifeComponent(ParticleTextGenerator::GeneratorParams const& in_params) :
		GameHUDCacheValueTextComponent<int>("%d", in_params) {}


protected:

	/** override */
	virtual bool QueryValue(int& result) const override;
	/** override */
	virtual void UpdateMesh() override;
};



