#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>

#include <death/GameFramework.h>

namespace death
{

	// =============================================
	// Checkpoint
	// =============================================

	class Checkpoint : public chaos::ReferencedObject
	{
		DEATH_GAMEFRAMEWORK_ALLFRIENDS()

	public:

		/** collect information from current level */
		virtual bool CollectSaveData(GameInstance const * game_instance, GameLevelInstance const * level_instance, Player const * player);
		/** restore information into current game_instance */
		virtual bool ApplySaveData(GameInstance * game_instance, GameLevelInstance * level_instance, Player * player) const;

	};


}; // namespace death
