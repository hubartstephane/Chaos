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

protected:

	/** override */
	virtual bool FillHUDContent() override;
	/** override */
	virtual bool CreateHUDLayers() override;
};



