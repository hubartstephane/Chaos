#pragma once

#include "Ludum50.h"

namespace chaos
{
	namespace GameHUDKeys
	{
	};
};

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



