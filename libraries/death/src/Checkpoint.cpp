#include <death/Checkpoint.h>

namespace death
{

	// =================================================
	// Checkpoint
	// =================================================

	bool Checkpoint::CollectSaveData(GameInstance const * game_instance, GameLevelInstance const * level_instance, Player const * player)
	{
		return true;
	}

	bool Checkpoint::ApplySaveData(GameInstance * game_instance, GameLevelInstance * level_instance, Player * player) const
	{

		return true;
	}


}; // namespace death
