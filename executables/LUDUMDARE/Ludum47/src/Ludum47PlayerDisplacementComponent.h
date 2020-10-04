#pragma once


#include <chaos/StandardHeaders.h> 
#include <death/PlayerDisplacementComponent.h>

class LudumPlayerDisplacementComponent : public death::PlayerDisplacementComponent
{
	CHAOS_OBJECT_DECLARE_CLASS2(LudumPlayerDisplacementComponent, death::PlayerDisplacementComponent);

protected:

	/** override */
	virtual bool DoTick(float delta_time) override;


protected:

	glm::vec2 collision_direction = { 0.0f, 0.0f };
	float     collision_reaction_intensity = 0.0f;
};
