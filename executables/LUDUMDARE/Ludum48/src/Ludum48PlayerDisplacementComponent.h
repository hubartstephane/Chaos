#pragma once


#include "Ludum48.h"

class LudumPlayerDisplacementComponent : public PlayerDisplacementComponent
{
	CHAOS_DECLARE_OBJECT_CLASS2(LudumPlayerDisplacementComponent, PlayerDisplacementComponent);

public:

	glm::vec2 GetPawnVelocity() const { return { 0.0f, 0.0f }; }

	glm::vec2 GetPawnMaxVelocity() const { return { 0.0f, 0.0f };}

	virtual bool DoTick(float delta_time) override;
};
