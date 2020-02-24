#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/Tickable.h>

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

	};

}; // namespace death
