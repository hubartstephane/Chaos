#pragma once

#include <chaos/Chaos.h>

class LudumPlayerDisplacementComponent : public chaos::PlayerDisplacementComponent
{
	CHAOS_DECLARE_OBJECT_CLASS2(LudumPlayerDisplacementComponent, chaos::PlayerDisplacementComponent);

protected:

	/** override */
	virtual bool DoTick(float delta_time) override;





};
