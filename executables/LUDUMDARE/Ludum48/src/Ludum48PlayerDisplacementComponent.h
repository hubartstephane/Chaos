#pragma once


#include <chaos/Chaos.h>

class LudumPlayerDisplacementComponent : public chaos::PlayerDisplacementComponent
{
	CHAOS_DECLARE_OBJECT_CLASS2(LudumPlayerDisplacementComponent, chaos::PlayerDisplacementComponent);

public:

	glm::vec2 GetPawnVelocity() const { return pawn_velocity; }

	glm::vec2 GetPawnMaxVelocity() const { return pawn_max_velocity; }

	virtual bool DoTick(float delta_time) override;

	
	virtual bool SerializeIntoJSON(nlohmann::json& json_entry) const override;
	
	virtual bool SerializeFromJSON(nlohmann::json const& json_entry) override;


protected:

	/** the current velocity of the pawn (unit is tile size) */
	glm::vec2 pawn_velocity = glm::vec2(0.0f, 0.0f);

	/** the maximum velocity of the pawn (unit is tile size) */
	glm::vec2 pawn_max_velocity = glm::vec2(3.0f, 3.0f);



	/** the offset of the pawn relative to the 'on the grid situation ' |0 .. 1] */
	float pawn_offset = 0.0f;

	/** size of the ceils */
	glm::vec2 tile_size = { 32.0f, 32.0f };

	/** position of the pawn when he starts moving */
	glm::vec2 start_pawn_position = { 0.0f, 0.0f };

	bool pawn_resting = true;
};
