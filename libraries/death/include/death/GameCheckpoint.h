#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>

#include <death/GameFramework.h>

namespace death
{

	// =============================================
	// PlayerCheckpoint
	// =============================================

	class PlayerCheckpoint : public chaos::ReferencedObject
	{
		DEATH_GAMEFRAMEWORK_ALLFRIENDS()

	public:


	};

	// =============================================
	// LevelCheckpoint
	// =============================================

	class LevelCheckpoint : public chaos::ReferencedObject
	{
		DEATH_GAMEFRAMEWORK_ALLFRIENDS()

	public:


	};

	// =============================================
	// GameCheckpoint
	// =============================================

	class GameCheckpoint : public chaos::ReferencedObject
	{
		DEATH_GAMEFRAMEWORK_ALLFRIENDS()

	public:

		/** the data for the player */
		chaos::shared_ptr<PlayerCheckpoint> player_checkpoint;
		/** the data for the level */
		chaos::shared_ptr<LevelCheckpoint> level_checkpoint;
	};


}; // namespace death
