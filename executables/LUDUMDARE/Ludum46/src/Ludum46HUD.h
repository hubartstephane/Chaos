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



