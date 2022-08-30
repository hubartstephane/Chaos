#pragma once
#include "Ludum50PCH.h"
#include "Ludum50.h"


class LudumPlayerDisplacementComponent : public PlayerDisplacementComponent
{
	CHAOS_DECLARE_OBJECT_CLASS(LudumPlayerDisplacementComponent, PlayerDisplacementComponent);

protected:

	/** override */
	virtual bool DoTick(float delta_time) override;


};
