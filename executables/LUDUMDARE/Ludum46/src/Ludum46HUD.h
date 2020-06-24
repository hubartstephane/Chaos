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

	DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(Ludum);

	CHAOS_OBJECT_DECLARE_CLASS2(LudumPlayingHUD, death::PlayingHUD);

protected:

	/** override */
	virtual bool FillHUDContent() override;
	/** override */
	virtual bool CreateHUDLayers() override;
};



