#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/Tickable.h>
#include <chaos/ParticleManager.h>
#include <chaos/ParticleDefault.h>

#include <death/GameFramework.h>
#include <death/GameCheckpoint.h>
#include <death/GameGettersDecl.h>

namespace death
{

	// =============================================
	// PlayerPawn
	// =============================================

	class PlayerPawn : public chaos::Tickable, public CheckpointObject<PlayerPawnCheckpoint>
	{

		DEATH_GAMEFRAMEWORK_ALLFRIENDS()

	public:

		DEATH_GAMEGETTERS_DECLARE();

		/** constructor */
		PlayerPawn(Player* in_player);

		/** returns the player the pawn belongs to */
		chaos::AutoCastable<Player> GetPlayer() { return player; }
		/** returns the player the pawn belongs to */
		chaos::AutoConstCastable<Player> GetPlayer() const { return player; }

		/** Get the position of the pawn */
		virtual glm::vec2 GetPosition() const;
		/** Get the bounding box of the pawn */
		virtual chaos::box2 GetBox() const;
		/** Set the position of the pawn */
		virtual bool SetPosition(glm::vec2 const& position);
		/** Set the bounding box of the pawn */
		virtual bool SetBox(chaos::box2 const& box);

		/** get the pawn allocation */
		chaos::ParticleAllocationBase* GetAllocation() { return allocations.get(); }
		/** get the pawn allocation */
		chaos::ParticleAllocationBase const* GetAllocation() const { return allocations.get(); }
		/** set the pawn allocation */
		virtual void SetAllocation(chaos::ParticleAllocationBase* in_allocation);

		/** get pawn particle */
		chaos::ParticleDefault * GetParticle();
		chaos::ParticleDefault const* GetParticle() const;

	protected:

		/** override */
		virtual bool DoSaveIntoCheckpoint(PlayerPawnCheckpoint* checkpoint) const override;
		/** override */
		virtual bool DoLoadFromCheckpoint(PlayerPawnCheckpoint const* checkpoint) override;

	protected:

		/** the player that owns this pawn */
		Player* player = nullptr;
		/** the allocation for the pawn */
		chaos::shared_ptr<chaos::ParticleAllocationBase> allocations;

	};

}; // namespace death
