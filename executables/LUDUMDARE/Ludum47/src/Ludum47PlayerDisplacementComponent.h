#pragma once

#include <chaos/Chaos.h>

class LudumPlayerDisplacementComponent : public chaos::PlayerDisplacementComponent
{
	CHAOS_OBJECT_DECLARE_CLASS2(LudumPlayerDisplacementComponent, chaos::PlayerDisplacementComponent);


public:

	static bool ComputeBorderCollision(class ParticleBase& particle, class LudumLevelInstance* li, class CarData const& car_data);


protected:

	/** override */
	virtual bool DoTick(float delta_time) override;





};
