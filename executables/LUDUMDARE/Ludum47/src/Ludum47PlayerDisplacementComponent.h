#pragma once


#include <chaos/StandardHeaders.h> 
#include <death/PlayerDisplacementComponent.h>

class LudumPlayerDisplacementComponent : public death::PlayerDisplacementComponent
{
	CHAOS_OBJECT_DECLARE_CLASS2(LudumPlayerDisplacementComponent, death::PlayerDisplacementComponent);

protected:

	/** override */
	virtual bool DoTick(float delta_time) override;

};
