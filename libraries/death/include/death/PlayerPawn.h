#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Object.h>
#include <chaos/Tickable.h>
#include <chaos/ParticleManager.h>
#include <chaos/ParticleDefault.h>
#include <chaos/JSONSerializable.h>

#include <death/GameFramework.h>
#include <death/GameGettersDecl.h>
#include <death/GameEntity.h>

namespace death
{

	// =============================================
	// PlayerPawn
	// =============================================

	class PlayerPawn : public GameEntity
	{

		DEATH_GAMEFRAMEWORK_ALLFRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(PlayerPawn, GameEntity);

	public:

		DEATH_GAMEGETTERS_DECLARE();

		/** initialization method */
		bool Initialize(Player* in_player);

		/** returns the player the pawn belongs to */
		chaos::AutoCastable<Player> GetPlayer() { return player; }
		/** returns the player the pawn belongs to */
		chaos::AutoConstCastable<Player> GetPlayer() const { return player; }

	protected:

		/** the player that owns this pawn */
		Player* player = nullptr;
	};

}; // namespace death
