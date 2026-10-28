#pragma once

#include "Ludum47PCH.h"

class LudumPlayerDisplacementComponent : public chaos::PlayerDisplacementComponent
{
	CHAOS_DECLARE_OBJECT_CLASS(LudumPlayerDisplacementComponent, chaos::PlayerDisplacementComponent);


public:

	static bool ComputeBorderCollision(class ParticleBase& particle, class LudumLevelInstance* li, class CarData const& car_data);

	virtual bool EnumerateInputActions(chaos::InputActionProcessor& in_action_processor, chaos::EnumerateInputActionContext in_context) override;

protected:

	/** override */
	virtual bool DoTick(float delta_time) override;

	class ParticlePlayer* GetParticlePlayer();



};
