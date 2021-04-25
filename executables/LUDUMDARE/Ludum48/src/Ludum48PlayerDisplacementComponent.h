#pragma once


#include <chaos/Chaos.h>

class LudumPlayerDisplacementComponent : public chaos::PlayerDisplacementComponent
{
	CHAOS_DECLARE_OBJECT_CLASS2(LudumPlayerDisplacementComponent, chaos::PlayerDisplacementComponent);

public:

	glm::vec2 GetPawnVelocity() const { return { 0.0f, 0.0f }; }

	glm::vec2 GetPawnMaxVelocity() const { return { 0.0f, 0.0f };}

	virtual bool DoTick(float delta_time) override;

	
	virtual bool SerializeIntoJSON(nlohmann::json& json_entry) const override;
	
	virtual bool SerializeFromJSON(nlohmann::json const& json_entry) override;


public:

	float pawn_speed = 3.0f;

	float push_speed = 1.0f;
};
