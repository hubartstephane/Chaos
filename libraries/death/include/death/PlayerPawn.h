#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/Tickable.h>
#include <chaos/ParticleManager.h>
#include <chaos/ParticleDefault.h>

#include <death/GameFramework.h>
#include <death/GameCheckpoint.h>
#include <death/GameInstanceEntity.h>

namespace death
{

	// =============================================
	// PlayerPawn
	// =============================================

	class PlayerPawn : public chaos::Tickable, public CheckpointObject<PlayerPawnCheckpoint>
	{

		DEATH_GAMEFRAMEWORK_ALLFRIENDS()

	public:

		DEATH_GAMEENTITY_IMPLEMENT();

		/** override */
		virtual glm::vec2 GetPosition() const;
		/** override */
		virtual chaos::box2 GetBox() const;
		/** override */
		virtual bool SetPosition(glm::vec2 const& position);
		/** override */
		virtual bool SetBox(chaos::box2 const& box);

		/** get the pawn allocation */
		chaos::ParticleAllocationBase* GetAllocation() { return allocations.get(); }
		/** get the pawn allocation */
		chaos::ParticleAllocationBase const* GetAllocation() const { return allocations.get(); }
		/** set the pawn allocation */
		virtual void SetAllocation(chaos::ParticleAllocationBase* in_allocation);

		/** get pawn particle */
		chaos::ParticleDefault::Particle * GetParticle();
		chaos::ParticleDefault::Particle const* GetParticle() const;

	protected:

		/** the allocation for the pawn */
		chaos::shared_ptr<chaos::ParticleAllocationBase> allocations;
	};

}; // namespace death
