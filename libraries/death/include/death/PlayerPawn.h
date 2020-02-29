#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/Tickable.h>
#include <chaos/ParticleManager.h>
#include <chaos/ParticleDefault.h>

#include <death/GameFramework.h>
#include <death/GameCheckpoint.h>

namespace death
{

	// =============================================
	// PlayerPawn
	// =============================================

	class PlayerPawn : public chaos::Tickable, public CheckpointObject<PlayerPawnCheckpoint>
	{
		DEATH_GAMEFRAMEWORK_ALLFRIENDS()

	public:

		/** destructor */
		virtual ~PlayerPawn() = default;

		/** get pawn position */
		virtual glm::vec2 GetPosition() const;
		/** get pawn box */
		virtual chaos::box2 GetBox() const;
		/** set the pawn position */
		virtual bool SetPosition(glm::vec2 const& position);
		/** set the pawn box */
		virtual bool SetBox(chaos::box2 const& box);
	};

	// =============================================
	// SimplePlayerPawn : a pawn to handle a single allocation
	// =============================================

	class SimplePlayerPawn : public PlayerPawn
	{

	public:

		/** override */
		virtual glm::vec2 GetPosition() const override;
		/** override */
		virtual chaos::box2 GetBox() const override;
		/** override */
		virtual bool SetPosition(glm::vec2 const& position) override;
		/** override */
		virtual bool SetBox(chaos::box2 const& box) override;

		/** get the pawn allocation */
		chaos::ParticleAllocationBase* GetAllocation() { return allocations.get(); }
		/** get the pawn allocation */
		chaos::ParticleAllocationBase const* GetAllocation() const { return allocations.get(); }
		/** set the pawn allocation */
		virtual void SetAllocation(chaos::ParticleAllocationBase* in_allocation);

		/** get pawn particle */
		chaos::ParticleDefault::Particle* GetParticle();
		chaos::ParticleDefault::Particle const* GetParticle() const;

	protected:

		/** the allocation for the pawn */
		chaos::shared_ptr<chaos::ParticleAllocationBase> allocations;
	};

}; // namespace death
