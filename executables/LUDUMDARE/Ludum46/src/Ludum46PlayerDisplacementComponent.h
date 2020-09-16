#pragma once

#include <chaos/StandardHeaders.h> 
#include <death/PlayerDisplacementComponent.h>

class LudumPlayerDisplacementComponent : public death::PlayerDisplacementComponent
{

public:


	DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(Ludum);

	CHAOS_OBJECT_DECLARE_CLASS2(LudumPlayerDisplacementComponent, death::PlayerDisplacementComponent);

	/** constructor */
	using death::PlayerDisplacementComponent::PlayerDisplacementComponent;

	/** override */
	virtual bool DoTick(float delta_time) override;

protected:

	chaos::box2 ComputeCollisions(chaos::box2 const& src_box, chaos::box2 const& dst_box, int collision_mask, chaos::ParticleAllocationBase* ignore_allocation, char const* wangset_name, std::function<void(chaos::box2 const&, death::TMParticle&, chaos::Edge)> func);

};
