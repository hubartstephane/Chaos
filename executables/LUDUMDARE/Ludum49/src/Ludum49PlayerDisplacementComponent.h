#pragma once

#include "Ludum49.h"

class LudumPlayerDisplacementComponent : public PlayerDisplacementComponent
{
	CHAOS_DECLARE_OBJECT_CLASS2(LudumPlayerDisplacementComponent, PlayerDisplacementComponent);

protected:

	/** override */
	virtual bool DoTick(float delta_time) override;




	glm::vec2 pawn_velocity = glm::vec2(0.0f, 0.0f);
};
